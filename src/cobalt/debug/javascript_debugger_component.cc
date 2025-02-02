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

#include "cobalt/debug/javascript_debugger_component.h"

#include "base/bind.h"
#include "base/optional.h"
#include "base/stringprintf.h"
#include "base/values.h"

namespace cobalt {
namespace debug {

namespace {
// Command, parameter and event names as specified by the protocol:
// https://chromedevtools.github.io/devtools-protocol/1-3/Debugger

// Parameter names.
const char kCallFrameId[] = "callFrameId";
const char kCallFrames[] = "callFrames";
const char kColumnNumber[] = "columnNumber";
const char kErrorLine[] = "errorLine";
const char kErrorMessage[] = "errorMessage";
const char kFunctionName[] = "functionName";
const char kLineNumber[] = "lineNumber";
const char kLocationColumnNumber[] = "location.columnNumber";
const char kLocationLineNumber[] = "location.lineNumber";
const char kLocationScriptId[] = "location.scriptId";
const char kObject[] = "object";
const char kReason[] = "reason";
const char kScopeChain[] = "scopeChain";
const char kScriptId[] = "scriptId";
const char kState[] = "state";
const char kThis[] = "this";
const char kType[] = "type";
const char kUrl[] = "url";

// Parameter values.
const char kDebugCommand[] = "debugCommand";

// Result parameters.
const char kBreakpointId[] = "result.breakpointId";
const char kLocations[] = "result.locations";
const char kScriptSource[] = "result.scriptSource";

// Events.
const char kPaused[] = "Debugger.paused";
const char kResumed[] = "Debugger.resumed";
const char kScriptFailedToParse[] = "Debugger.scriptFailedToParse";
const char kScriptParsed[] = "Debugger.scriptParsed";

// Construct a unique breakpoint id from url and source location.
// Use the same format as Chrome.
std::string BreakpointId(const std::string url, int line_number,
                         int column_number) {
  return base::StringPrintf("%s:%d:%d", url.c_str(), line_number,
                            column_number);
}

}  // namespace

JavaScriptDebuggerComponent::JavaScriptDebuggerComponent(
    DebugDispatcher* dispatcher)
    : dispatcher_(dispatcher),
      source_providers_deleter_(&source_providers_),
      ALLOW_THIS_IN_INITIALIZER_LIST(commands_(this)) {
  DCHECK(dispatcher_);

  commands_["Debugger.disable"] = &JavaScriptDebuggerComponent::Disable;
  commands_["Debugger.enable"] = &JavaScriptDebuggerComponent::Enable;
  commands_["Debugger.getScriptSource"] =
      &JavaScriptDebuggerComponent::GetScriptSource;
  commands_["Debugger.pause"] = &JavaScriptDebuggerComponent::Pause;
  commands_["Debugger.resume"] = &JavaScriptDebuggerComponent::Resume;
  commands_["Debugger.stepInto"] = &JavaScriptDebuggerComponent::StepInto;
  commands_["Debugger.setBreakpointByUrl"] =
      &JavaScriptDebuggerComponent::SetBreakpointByUrl;
  commands_["Debugger.setPauseOnExceptions"] =
      &JavaScriptDebuggerComponent::SetPauseOnExceptions;
  commands_["Debugger.stepOut"] = &JavaScriptDebuggerComponent::StepOut;
  commands_["Debugger.stepOver"] = &JavaScriptDebuggerComponent::StepOver;

  dispatcher_->AddDomain("Debugger", commands_.Bind());
}

JavaScriptDebuggerComponent::~JavaScriptDebuggerComponent() {}

void JavaScriptDebuggerComponent::Enable(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->Attach();
  command.SendResponse();
}

void JavaScriptDebuggerComponent::Disable(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->Detach();
  command.SendResponse();
}

void JavaScriptDebuggerComponent::GetScriptSource(const Command& command) {
  JSONObject params = JSONParse(command.GetParams());
  // Get the scriptId from the parameters.
  std::string script_id;
  bool got_script_id = params->GetString(kScriptId, &script_id);
  if (!got_script_id) {
    command.SendErrorResponse("No scriptId specified in parameters.");
    return;
  }

  // Find the source provider with a matching scriptId.
  SourceProviderMap::iterator it = source_providers_.find(script_id);
  if (it == source_providers_.end()) {
    command.SendErrorResponse("No script found with specified scriptId.");
    return;
  }
  script::SourceProvider* source_provider = it->second;
  DCHECK(source_provider);

  // Build and return the result.
  JSONObject result(new base::DictionaryValue());
  result->SetString(kScriptSource, source_provider->GetScriptSource());
  command.SendResponse(result);
}

void JavaScriptDebuggerComponent::Pause(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->Pause();
  command.SendResponse();
}

void JavaScriptDebuggerComponent::Resume(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->Resume();
  dispatcher_->SetPaused(false);
  command.SendResponse();
}

void JavaScriptDebuggerComponent::SetBreakpointByUrl(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  JSONObject params = JSONParse(command.GetParams());

  std::string url;
  bool got_url = params->GetString(kUrl, &url);
  if (!got_url) {
    command.SendErrorResponse("Breakpoint URL must be specified.");
    return;
  }

  // TODO: Should also handle setting of breakpoint by urlRegex

  int line_number;
  bool got_line_number = params->GetInteger(kLineNumber, &line_number);
  if (!got_line_number) {
    command.SendErrorResponse("Line number must be specified.");
    return;
  }
  // If no column number is specified, just default to 0.
  int column_number = 0;
  params->GetInteger(kColumnNumber, &column_number);

  // TODO: Should also handle condition and isAntibreakpoint.

  // Create a new logical breakpoint and store it in our map.
  const std::string breakpoint_id =
      BreakpointId(url, line_number, column_number);
  Breakpoint breakpoint(url, line_number, column_number);
  breakpoints_[breakpoint_id] = breakpoint;

  // Check the logical breakpoint against all currently loaded source providers
  // and get an array of matching breakpoint locations.
  std::vector<ScriptLocation> locations;
  ResolveBreakpoint(breakpoint, &locations);

  // Construct a result object from the logical breakpoint id and resolved
  // source locations.
  JSONObject result(new base::DictionaryValue());
  result->SetString(kBreakpointId, breakpoint_id);
  JSONList location_objects(new base::ListValue());
  for (std::vector<ScriptLocation>::const_iterator it = locations.begin();
       it != locations.end(); ++it) {
    JSONObject location(new base::DictionaryValue());
    location->SetString(kScriptId, it->script_id);
    location->SetInteger(kLineNumber, it->line_number);
    location->SetInteger(kColumnNumber, it->column_number);
    location_objects->Append(location.release());
  }
  result->Set(kLocations, location_objects.release());
  command.SendResponse(result);
}

void JavaScriptDebuggerComponent::SetPauseOnExceptions(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  JSONObject params = JSONParse(command.GetParams());

  std::string state;
  DCHECK(params->GetString(kState, &state));
  if (state == "all") {
    dispatcher_->script_debugger()->SetPauseOnExceptions(
        script::ScriptDebugger::kAll);
  } else if (state == "none") {
    dispatcher_->script_debugger()->SetPauseOnExceptions(
        script::ScriptDebugger::kNone);
  } else if (state == "uncaught") {
    dispatcher_->script_debugger()->SetPauseOnExceptions(
        script::ScriptDebugger::kUncaught);
  } else {
    NOTREACHED();
  }
  command.SendResponse();
}

void JavaScriptDebuggerComponent::StepInto(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->StepInto();
  dispatcher_->SetPaused(false);
  command.SendResponse();
}

void JavaScriptDebuggerComponent::StepOut(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->StepOut();
  dispatcher_->SetPaused(false);
  command.SendResponse();
}

void JavaScriptDebuggerComponent::StepOver(const Command& command) {
  DCHECK(dispatcher_->script_debugger());
  dispatcher_->script_debugger()->StepOver();
  dispatcher_->SetPaused(false);
  command.SendResponse();
}

void JavaScriptDebuggerComponent::OnScriptDebuggerDetach(
    const std::string& reason) {
  DLOG(INFO) << "JavaScript debugger detached: " << reason;
}

void JavaScriptDebuggerComponent::OnScriptDebuggerPause(
    scoped_ptr<script::CallFrame> call_frame) {
  // Notify the clients we're about to pause.
  SendPausedEvent(call_frame.Pass());

  // Tell the debug dispatcher to enter paused state - block this thread.
  dispatcher_->SetPaused(true);

  // Notify the clients we've resumed.
  SendResumedEvent();
}

void JavaScriptDebuggerComponent::OnScriptFailedToParse(
    scoped_ptr<script::SourceProvider> source_provider) {
  DCHECK(source_provider);
  HandleScriptEvent(kScriptFailedToParse, source_provider.Pass());
}

void JavaScriptDebuggerComponent::OnScriptParsed(
    scoped_ptr<script::SourceProvider> source_provider) {
  DCHECK(source_provider);
  HandleScriptEvent(kScriptParsed, source_provider.Pass());
}

JSONObject JavaScriptDebuggerComponent::CreateCallFrameData(
    const scoped_ptr<script::CallFrame>& call_frame) {
  DCHECK(call_frame);

  // Create the JSON object and add the data for this call frame.
  JSONObject call_frame_data(new base::DictionaryValue());
  call_frame_data->SetString(kCallFrameId, call_frame->GetCallFrameId());
  call_frame_data->SetString(kFunctionName, call_frame->GetFunctionName());

  // Offset the line number according to the start line of the source.
  const std::string script_id = call_frame->GetScriptId();
  int line_number = call_frame->GetLineNumber();
  DCHECK(source_providers_[script_id]);
  base::optional<int> start_line = source_providers_[script_id]->GetStartLine();
  line_number -= start_line.value_or(1);

  // Add the location data.
  call_frame_data->SetString(kLocationScriptId, script_id);
  call_frame_data->SetInteger(kLocationLineNumber, line_number);
  base::optional<int> column_number = call_frame->GetColumnNumber();
  if (column_number) {
    call_frame_data->SetInteger(kLocationColumnNumber, column_number.value());
  }

  // Add the scope chain data.
  JSONList scope_chain_data(CreateScopeChainData(call_frame->GetScopeChain()));
  call_frame_data->Set(kScopeChain, scope_chain_data.release());

  // Add the "this" object data.
  const script::ValueHandleHolder* this_object = call_frame->GetThis();
  if (this_object) {
    JSONObject this_object_data(dispatcher_->CreateRemoteObject(this_object));
    call_frame_data->Set(kThis, this_object_data.release());
  }

  return call_frame_data.Pass();
}

JSONList JavaScriptDebuggerComponent::CreateCallStackData(
    scoped_ptr<script::CallFrame> call_frame) {
  JSONList call_frame_list(new base::ListValue());

  // Consume the scoped CallFrame objects as we iterate over them.
  while (call_frame) {
    JSONObject call_frame_data(CreateCallFrameData(call_frame));
    DCHECK(call_frame_data);
    call_frame_list->Append(call_frame_data.release());
    scoped_ptr<script::CallFrame> next_call_frame(call_frame->GetCaller());
    call_frame = next_call_frame.Pass();
  }

  return call_frame_list.Pass();
}

JSONObject JavaScriptDebuggerComponent::CreateScopeData(
    const scoped_ptr<script::Scope>& scope) {
  DCHECK(scope);
  const script::ValueHandleHolder* scope_object = scope->GetObject();
  JSONObject scope_data(new base::DictionaryValue());
  scope_data->Set(kObject,
                  dispatcher_->CreateRemoteObject(scope_object).release());
  scope_data->SetString(kType, script::Scope::TypeToString(scope->GetType()));
  return scope_data.Pass();
}

JSONList JavaScriptDebuggerComponent::CreateScopeChainData(
    scoped_ptr<script::Scope> scope) {
  JSONList scope_chain_list(new base::ListValue());

  // Consume the scoped Scope objects as we iterate over them.
  while (scope) {
    JSONObject scope_data(CreateScopeData(scope));
    DCHECK(scope_data);
    scope_chain_list->Append(scope_data.release());
    scoped_ptr<script::Scope> next_scope(scope->GetNext());
    scope = next_scope.Pass();
  }

  return scope_chain_list.Pass();
}

void JavaScriptDebuggerComponent::HandleScriptEvent(
    const std::string& method,
    scoped_ptr<script::SourceProvider> source_provider) {
  DCHECK(source_provider);

  // Send the event notification to the debugger clients.
  JSONObject params(new base::DictionaryValue());
  params->SetString(kScriptId, source_provider->GetScriptId());
  params->SetString(kUrl, source_provider->GetUrl());
  base::optional<int> error_line = source_provider->GetErrorLine();
  if (error_line) {
    DCHECK_EQ(method, kScriptFailedToParse);
    params->SetInteger(kErrorLine, error_line.value());
  }
  base::optional<std::string> error_message =
      source_provider->GetErrorMessage();
  if (error_message) {
    DCHECK_EQ(method, kScriptFailedToParse);
    params->SetString(kErrorMessage, error_message.value());
  }
  dispatcher_->SendEvent(method, params);

  // Store the raw pointer to the source provider in the map.
  // The values in the map will be deleted on destruction by
  // |source_providers_deleter_|.
  const std::string script_id = source_provider->GetScriptId();
  SourceProviderMap::iterator it = source_providers_.find(script_id);
  if (it != source_providers_.end()) {
    delete it->second;
  }
  source_providers_[script_id] = source_provider.release();
}

void JavaScriptDebuggerComponent::ResolveBreakpoint(
    const Breakpoint& breakpoint, std::vector<ScriptLocation>* locations) {
  for (SourceProviderMap::iterator it = source_providers_.begin();
       it != source_providers_.end(); ++it) {
    script::SourceProvider* script = it->second;
    if (script->GetUrl() == breakpoint.url) {
      dispatcher_->script_debugger()->SetBreakpoint(
          script->GetScriptId(),
          breakpoint.line_number + script->GetStartLine().value_or(1),
          breakpoint.column_number);
      locations->push_back(ScriptLocation(script->GetScriptId(),
                                          breakpoint.line_number,
                                          breakpoint.column_number));
    }
  }
}

void JavaScriptDebuggerComponent::SendPausedEvent(
    scoped_ptr<script::CallFrame> call_frame) {
  std::string event_method = kPaused;
  JSONObject event_params(new base::DictionaryValue());
  JSONList call_stack_data(CreateCallStackData(call_frame.Pass()));
  DCHECK(call_stack_data);
  event_params->Set(kCallFrames, call_stack_data.release());
  event_params->SetString(kReason, kDebugCommand);
  dispatcher_->SendEvent(event_method, event_params);
}

void JavaScriptDebuggerComponent::SendResumedEvent() {
  // Send the event to the clients. No parameters.
  std::string event_method = kResumed;
  scoped_ptr<base::DictionaryValue> event_params(new base::DictionaryValue());
  dispatcher_->SendEvent(event_method, event_params);
}

}  // namespace debug
}  // namespace cobalt
