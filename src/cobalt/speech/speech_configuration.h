// Copyright 2016 The Cobalt Authors. All Rights Reserved.
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

#ifndef COBALT_SPEECH_SPEECH_CONFIGURATION_H_
#define COBALT_SPEECH_SPEECH_CONFIGURATION_H_

#include "build/build_config.h"
#include "starboard/configuration.h"

#if SB_HAS(MICROPHONE)
#define SB_USE_SB_MICROPHONE 1
#endif  // SB_HAS(MICROPHONE)

#if SB_HAS(SPEECH_RECOGNIZER) && SB_API_VERSION >= 5
#define SB_USE_SB_SPEECH_RECOGNIZER 1
#endif  // SB_HAS(SPEECH_RECOGNIZER) && SB_API_VERSION >= 5

#endif  // COBALT_SPEECH_SPEECH_CONFIGURATION_H_
