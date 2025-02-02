// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_loop_proxy_impl.h"

#include "base/location.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread_restrictions.h"

namespace base {

MessageLoopProxyImpl::~MessageLoopProxyImpl() {
}

bool MessageLoopProxyImpl::PostDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay) {
  return PostTaskHelper(from_here, task, delay, true);
}

bool MessageLoopProxyImpl::PostNonNestableDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay) {
  return PostTaskHelper(from_here, task, delay, false);
}

#if defined(COBALT)
namespace {
// Runs the given task, and then signals the given WaitableEvent.
void RunAndSignal(const Closure& task, WaitableEvent* event) {
  task.Run();
  event->Signal();
}
}  // namespace

bool MessageLoopProxyImpl::PostBlockingTask(
    const tracked_objects::Location& from_here,
    const Closure& task) {
  WaitableEvent task_finished(true /* manual reset */,
                              false /* initially unsignaled */);
  {
    AutoLock lock(message_loop_lock_);
    if (!target_message_loop_) {
      return false;
    }

    target_message_loop_->PostTask(
        from_here,
        Bind(&RunAndSignal, task, Unretained(&task_finished)));
  }

  // Outside of the lock, wait for the task to complete before proceeding.
  task_finished.Wait();
  return true;
}
#endif

bool MessageLoopProxyImpl::RunsTasksOnCurrentThread() const {
  return thread_id_ == PlatformThread::CurrentId();
}

// MessageLoop::DestructionObserver implementation
void MessageLoopProxyImpl::WillDestroyCurrentMessageLoop() {
  AutoLock lock(message_loop_lock_);
  target_message_loop_ = NULL;
}

void MessageLoopProxyImpl::OnDestruct() const {
  // We shouldn't use MessageLoop::current() since it uses LazyInstance which
  // may be deleted by ~AtExitManager when a WorkerPool thread calls this
  // function.
  // http://crbug.com/63678
  bool delete_later = false;
  {
    AutoLock lock(message_loop_lock_);
    if (target_message_loop_) {
      base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
      if (MessageLoop::current() != target_message_loop_) {
        target_message_loop_->DeleteSoon(FROM_HERE, this);
        delete_later = true;
      }
    }
  }
  if (!delete_later)
    delete this;
}

MessageLoopProxyImpl::MessageLoopProxyImpl()
    : thread_id_(PlatformThread::CurrentId()),
      target_message_loop_(MessageLoop::current()) {}

bool MessageLoopProxyImpl::PostTaskHelper(
    const tracked_objects::Location& from_here, const base::Closure& task,
    base::TimeDelta delay, bool nestable) {
  AutoLock lock(message_loop_lock_);
  if (target_message_loop_) {
    if (nestable) {
      target_message_loop_->PostDelayedTask(from_here, task, delay);
    } else {
      target_message_loop_->PostNonNestableDelayedTask(from_here, task, delay);
    }
    return true;
  }
  return false;
}

scoped_refptr<MessageLoopProxy>
MessageLoopProxy::current() {
  MessageLoop* cur_loop = MessageLoop::current();
  if (!cur_loop)
    return NULL;
  return cur_loop->message_loop_proxy();
}

}  // namespace base
