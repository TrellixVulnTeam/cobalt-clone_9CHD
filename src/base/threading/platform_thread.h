// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// WARNING: You should *NOT* be using this class directly.  PlatformThread is
// the low-level platform-specific abstraction to the OS's threading interface.
// You should instead be using a message-loop driven Thread, see thread.h.

#ifndef BASE_THREADING_PLATFORM_THREAD_H_
#define BASE_THREADING_PLATFORM_THREAD_H_

#include "base/base_export.h"
#include "base/basictypes.h"
#include "base/time.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_POSIX)
#include <pthread.h>
#include <unistd.h>
#elif defined(OS_STARBOARD)
#include "starboard/thread.h"
#endif

namespace base {

// PlatformThreadHandle should not be assumed to be a numeric type, since the
// standard intends to allow pthread_t to be a structure.  This means you
// should not initialize it to a value, like 0.  If it's a member variable, the
// constructor can safely "value initialize" using () in the initializer list.
//
// ... Note that PlatformThreadHandle is assumed to be a scalar type below.
#if defined(OS_WIN)
typedef DWORD PlatformThreadId;
typedef void* PlatformThreadHandle;  // HANDLE
const PlatformThreadHandle kNullThreadHandle = NULL;
const PlatformThreadId kInvalidThreadId = 0;
#elif defined(OS_POSIX)
typedef pthread_t PlatformThreadHandle;
const PlatformThreadHandle kNullThreadHandle = 0;
typedef pid_t PlatformThreadId;
const PlatformThreadId kInvalidThreadId = 0;
#if defined(COBALT)
typedef int ThreadAffinity;
const ThreadAffinity kNoThreadAffinity = -1;
#endif
#elif defined(OS_STARBOARD)
typedef SbThread PlatformThreadHandle;
const PlatformThreadHandle kNullThreadHandle = kSbThreadInvalid;
typedef SbThreadId PlatformThreadId;
const PlatformThreadId kInvalidThreadId = kSbThreadInvalidId;
typedef SbThreadAffinity ThreadAffinity;
const ThreadAffinity kNoThreadAffinity = kSbThreadNoAffinity;
#endif

// Valid values for SetThreadPriority()
enum ThreadPriority {
#if defined(OS_STARBOARD)
  // See the priorities defined in "starboard/thread.h"

  kThreadPriority_Default = kSbThreadNoPriority,
  kThreadPriority_Lowest = kSbThreadPriorityLowest,
  kThreadPriority_Low = kSbThreadPriorityLow,
  kThreadPriority_Normal = kSbThreadPriorityNormal,
  kThreadPriority_High = kSbThreadPriorityHigh,
  kThreadPriority_Highest = kSbThreadPriorityHighest,
  kThreadPriority_RealTime = kSbThreadPriorityRealTime,
  kThreadPriority_RealtimeAudio = kThreadPriority_RealTime,
#elif defined(COBALT)
  // Cobalt on non-Starboard platforms.

  kThreadPriority_Default = -1,
  kThreadPriority_Lowest = 0,
  kThreadPriority_Low,
  kThreadPriority_Normal,
  kThreadPriority_High,
  kThreadPriority_Highest,
  kThreadPriority_RealTime,
  kThreadPriority_RealtimeAudio = kThreadPriority_RealTime,
#else
  // The original Chromium priorities.

  kThreadPriority_Normal,
  // Suitable for low-latency, glitch-resistant audio.
  kThreadPriority_RealtimeAudio,
#endif
};

// A namespace for low-level thread functions.
class BASE_EXPORT PlatformThread {
 public:
  // Implement this interface to run code on a background thread.  Your
  // ThreadMain method will be called on the newly created thread.
  class BASE_EXPORT Delegate {
   public:
    virtual void ThreadMain() = 0;

   protected:
    virtual ~Delegate() {}
  };

#if defined(__LB_SHELL__) || defined(OS_STARBOARD)
  struct PlatformThreadOptions {
    // <= 0 indicates default stack size
    int64_t stack_size;

    // The priority to start the thread at, or kThreadPriority_Default for the
    // default priority assignment policy.
    ThreadPriority priority;

    // The CPU index to pin the thread to, or kNoThreadAffinity for no affinity.
    ThreadAffinity affinity;
  };
#endif

  // Gets the current thread id, which may be useful for logging purposes.
  static PlatformThreadId CurrentId();

  // Yield the current thread so another thread can be scheduled.
  static void YieldCurrentThread();

  // Sleeps for the specified duration.
  static void Sleep(base::TimeDelta duration);

  // Sets the thread name visible to debuggers/tools. This has no effect
  // otherwise. This name pointer is not copied internally. Thus, it must stay
  // valid until the thread ends.
  static void SetName(const char* name);

  // Gets the thread name, if previously set by SetName.
  static const char* GetName();

  // Creates a new thread.  The |stack_size| parameter can be 0 to indicate
  // that the default stack size should be used.  Upon success,
  // |*thread_handle| will be assigned a handle to the newly created thread,
  // and |delegate|'s ThreadMain method will be executed on the newly created
  // thread.
  // NOTE: When you are done with the thread handle, you must call Join to
  // release system resources associated with the thread.  You must ensure that
  // the Delegate object outlives the thread.
  static bool Create(size_t stack_size, Delegate* delegate,
                     PlatformThreadHandle* thread_handle);

  // CreateWithPriority() does the same thing as Create() except the priority of
  // the thread is set based on |priority|.  Can be used in place of Create()
  // followed by SetThreadPriority().  SetThreadPriority() has not been
  // implemented on the Linux platform yet, this is the only way to get a high
  // priority thread on Linux.
  static bool CreateWithPriority(size_t stack_size, Delegate* delegate,
                                 PlatformThreadHandle* thread_handle,
                                 ThreadPriority priority);

  // CreateNonJoinable() does the same thing as Create() except the thread
  // cannot be Join()'d.  Therefore, it also does not output a
  // PlatformThreadHandle.
  static bool CreateNonJoinable(size_t stack_size, Delegate* delegate);

#if defined(__LB_SHELL__) || defined(OS_STARBOARD)
  static bool CreateWithOptions(const PlatformThreadOptions& options,
                                Delegate* delegate,
                                PlatformThreadHandle* thread_handle);
#endif

  // Joins with a thread created via the Create function.  This function blocks
  // the caller until the designated thread exits.  This will invalidate
  // |thread_handle|.
  static void Join(PlatformThreadHandle thread_handle);

  // Sets the priority of the thread specified in |handle| to |priority|.
  // This does not work on Linux, use CreateWithPriority() instead.
  static void SetThreadPriority(PlatformThreadHandle handle,
                                ThreadPriority priority);

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(PlatformThread);
};

}  // namespace base

#endif  // BASE_THREADING_PLATFORM_THREAD_H_
