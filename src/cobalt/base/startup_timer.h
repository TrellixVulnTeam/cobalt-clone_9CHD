// Copyright 2017 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COBALT_BASE_STARTUP_TIMER_H_
#define COBALT_BASE_STARTUP_TIMER_H_

#include "base/time.h"

namespace base {
namespace StartupTimer {

// Returns the time when the app started.
TimeTicks StartTime();

// Returns the time elapsed since the app started.
TimeDelta TimeElapsed();

}  // namespace StartupTimer
}  // namespace base

#endif  // COBALT_BASE_STARTUP_TIMER_H_
