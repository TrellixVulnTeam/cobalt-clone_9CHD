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

#include "cobalt/debug/runtime_component.h"

#include "base/bind.h"

namespace cobalt {
namespace debug {

namespace {
// File to load JavaScript runtime implementation from.
const char kScriptFile[] = "runtime.js";

// Event "methods" (names):
const char kExecutionContextCreated[] = "Runtime.executionContextCreated";
}  // namespace

RuntimeComponent::RuntimeComponent(DebugDispatcher* dispatcher)
    : dispatcher_(dispatcher), ALLOW_THIS_IN_INITIALIZER_LIST(commands_(this)) {
  DCHECK(dispatcher_);
  if (!dispatcher_->RunScriptFile(kScriptFile)) {
    DLOG(WARNING) << "Cannot execute Runtime initialization script.";
  }

  commands_["Runtime.enable"] = &RuntimeComponent::Enable;
  commands_["Runtime.disable"] = &RuntimeComponent::Disable;
  commands_["Runtime.compileScript"] = &RuntimeComponent::CompileScript;

  dispatcher_->AddDomain("Runtime", commands_.Bind());
}

void RuntimeComponent::CompileScript(const Command& command) {
  // TODO: Parse the JS without eval-ing it... This is to support:
  // a) Multi-line input from the devtools console
  // b) https://developers.google.com/web/tools/chrome-devtools/snippets
  command.SendResponse();
}

void RuntimeComponent::Disable(const Command& command) {
  command.SendResponse();
}

void RuntimeComponent::Enable(const Command& command) {
  dispatcher_->SendScriptEvent(kExecutionContextCreated,
                               "runtime.executionContextCreatedEvent");
  command.SendResponse();
}

}  // namespace debug
}  // namespace cobalt
