// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// PLEASE READ BEFORE CHANGING THIS FILE!
//
// This file implements the support code for the out of bounds signal handler.
// Nothing in here actually runs in the signal handler, but the code here
// manipulates data structures used by the signal handler so we still need to be
// careful. In order to minimize this risk, here are some rules to follow.
//
// 1. Avoid introducing new external dependencies. The files in src/trap-handler
//    should be as self-contained as possible to make it easy to audit the code.
//
// 2. Any changes must be reviewed by someone from the crash reporting
//    or security team. Se OWNERS for suggested reviewers.
//
// For more information, see https://goo.gl/yMeyUY.
//
// For the code that runs in the signal handler itself, see handler-inside.cc.

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <atomic>
#include <limits>

#include "src/trap-handler/trap-handler-internal.h"
#include "src/trap-handler/trap-handler.h"

#if V8_OS_STARBOARD
#include "src/poems.h"
#endif

namespace {
size_t gNextCodeObject = 0;

#if defined(DEBUG)
const bool kEnableDebug = true;
#else
const bool kEnableDebug = false;
#endif
}

namespace v8 {
namespace internal {
namespace trap_handler {

const size_t kInitialCodeObjectSize = 1024;
const size_t kCodeObjectGrowthFactor = 2;

constexpr size_t HandlerDataSize(size_t num_protected_instructions) {
  return offsetof(CodeProtectionInfo, instructions) +
         num_protected_instructions * sizeof(ProtectedInstructionData);
}

namespace {
template <typename = std::enable_if<kEnableDebug>>
bool IsDisjoint(const CodeProtectionInfo* a, const CodeProtectionInfo* b) {
  if (a == nullptr || b == nullptr) {
    return true;
  }

  const auto a_base = reinterpret_cast<uintptr_t>(a->base);
  const auto b_base = reinterpret_cast<uintptr_t>(b->base);

  return a_base >= b_base + b->size || b_base >= a_base + a->size;
}

// Verify that the code range does not overlap any that have already been
// registered.
void VerifyCodeRangeIsDisjoint(const CodeProtectionInfo* code_info) {
  for (size_t i = 0; i < gNumCodeObjects; ++i) {
    DCHECK(IsDisjoint(code_info, gCodeObjects[i].code_info));
  }
}

void ValidateCodeObjects() {
  // Sanity-check the code objects
  for (unsigned i = 0; i < gNumCodeObjects; ++i) {
    const auto* data = gCodeObjects[i].code_info;

    if (data == nullptr) continue;

    // Do some sanity checks on the protected instruction data
    for (unsigned i = 0; i < data->num_protected_instructions; ++i) {
      DCHECK_GE(data->instructions[i].instr_offset, 0);
      DCHECK_LT(data->instructions[i].instr_offset, data->size);
      DCHECK_GE(data->instructions[i].landing_offset, 0);
      DCHECK_LT(data->instructions[i].landing_offset, data->size);
      DCHECK_GT(data->instructions[i].landing_offset,
                data->instructions[i].instr_offset);
    }
  }

  // Check the validity of the free list.
  size_t free_count = 0;
  for (size_t i = gNextCodeObject; i != gNumCodeObjects;
       i = gCodeObjects[i].next_free) {
    DCHECK_LT(i, gNumCodeObjects);
    ++free_count;
    // This check will fail if we encounter a cycle.
    DCHECK_LE(free_count, gNumCodeObjects);
  }

  // Check that all free entries are reachable via the free list.
  size_t free_count2 = 0;
  for (size_t i = 0; i < gNumCodeObjects; ++i) {
    if (gCodeObjects[i].code_info == nullptr) {
      ++free_count2;
    }
  }
  DCHECK_EQ(free_count, free_count2);
}
}  // namespace

CodeProtectionInfo* CreateHandlerData(
    void* base, size_t size, size_t num_protected_instructions,
    const ProtectedInstructionData* protected_instructions) {
  const size_t alloc_size = HandlerDataSize(num_protected_instructions);
  CodeProtectionInfo* data =
      reinterpret_cast<CodeProtectionInfo*>(malloc(alloc_size));

  if (data == nullptr) {
    return nullptr;
  }

  data->base = base;
  data->size = size;
  data->num_protected_instructions = num_protected_instructions;

  memcpy(data->instructions, protected_instructions,
         num_protected_instructions * sizeof(ProtectedInstructionData));

  return data;
}

void UpdateHandlerDataCodePointer(int index, void* base) {
  MetadataLock lock;
  if (static_cast<size_t>(index) >= gNumCodeObjects) {
    abort();
  }
  CodeProtectionInfo* data = gCodeObjects[index].code_info;
  data->base = base;
}

int RegisterHandlerData(
    void* base, size_t size, size_t num_protected_instructions,
    const ProtectedInstructionData* protected_instructions) {
  // TODO(eholk): in debug builds, make sure this data isn't already registered.

  CodeProtectionInfo* data = CreateHandlerData(
      base, size, num_protected_instructions, protected_instructions);

  if (data == nullptr) {
    abort();
  }

  MetadataLock lock;

  if (kEnableDebug) {
    VerifyCodeRangeIsDisjoint(data);
  }

  size_t i = gNextCodeObject;

  // Explicitly convert std::numeric_limits<int>::max() to unsigned to avoid
  // compiler warnings about signed/unsigned comparisons. We aren't worried
  // about sign extension because we know std::numeric_limits<int>::max() is
  // positive.
  const size_t int_max = std::numeric_limits<int>::max();

  // We didn't find an opening in the available space, so grow.
  if (i == gNumCodeObjects) {
    size_t new_size = gNumCodeObjects > 0
                          ? gNumCodeObjects * kCodeObjectGrowthFactor
                          : kInitialCodeObjectSize;

    // Because we must return an int, there is no point in allocating space for
    // more objects than can fit in an int.
    if (new_size > int_max) {
      new_size = int_max;
    }
    if (new_size == gNumCodeObjects) {
      return kInvalidIndex;
    }

    // Now that we know our new size is valid, we can go ahead and realloc the
    // array.
    gCodeObjects = static_cast<CodeProtectionInfoListEntry*>(
        realloc(gCodeObjects, sizeof(*gCodeObjects) * new_size));

    if (gCodeObjects == nullptr) {
      abort();
    }

    memset(gCodeObjects + gNumCodeObjects, 0,
           sizeof(*gCodeObjects) * (new_size - gNumCodeObjects));
    for (size_t j = gNumCodeObjects; j < new_size; ++j) {
      gCodeObjects[j].next_free = j + 1;
    }
    gNumCodeObjects = new_size;
  }

  DCHECK(gCodeObjects[i].code_info == nullptr);

  // Find out where the next entry should go.
  gNextCodeObject = gCodeObjects[i].next_free;

  if (i <= int_max) {
    gCodeObjects[i].code_info = data;

    if (kEnableDebug) {
      ValidateCodeObjects();
    }

    return static_cast<int>(i);
  } else {
    return kInvalidIndex;
  }
}

void ReleaseHandlerData(int index) {
  if (index == kInvalidIndex) {
    return;
  }
  DCHECK_GE(index, 0);

  // Remove the data from the global list if it's there.
  CodeProtectionInfo* data = nullptr;
  {
    MetadataLock lock;

    data = gCodeObjects[index].code_info;
    gCodeObjects[index].code_info = nullptr;

    gCodeObjects[index].next_free = gNextCodeObject;
    gNextCodeObject = index;

    if (kEnableDebug) {
      ValidateCodeObjects();
    }
  }
  // TODO(eholk): on debug builds, ensure there are no more copies in
  // the list.
  DCHECK_NOT_NULL(data);  // make sure we're releasing legitimate handler data.
  free(data);
}

bool RegisterDefaultSignalHandler() {
#if V8_TRAP_HANDLER_SUPPORTED
  CHECK(!g_is_default_signal_handler_registered);

  struct sigaction action;
  action.sa_sigaction = HandleSignal;
  action.sa_flags = SA_SIGINFO;
  sigemptyset(&action.sa_mask);
  // {sigaction} installs a new custom segfault handler. On success, it returns
  // 0. If we get a nonzero value, we report an error to the caller by returning
  // false.
  if (sigaction(SIGSEGV, &action, &g_old_handler) != 0) {
    return false;
  }

  g_is_default_signal_handler_registered = true;
  return true;
#else
  return false;
#endif
}

size_t GetRecoveredTrapCount() {
  return gRecoveredTrapCount.load(std::memory_order_relaxed);
}

}  // namespace trap_handler
}  // namespace internal
}  // namespace v8
