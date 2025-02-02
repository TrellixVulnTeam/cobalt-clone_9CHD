// Copyright 2014 The Cobalt Authors. All Rights Reserved.
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

#include "cobalt/dom/dom_exception.h"

namespace cobalt {
namespace dom {
namespace {
const char* GetErrorName(DOMException::ExceptionCode code) {
  // Error names are enumerated here:
  //   http://heycam.github.io/webidl/#idl-DOMException-error-names
  switch (code) {
    case DOMException::kNone:
      return "";
    case DOMException::kIndexSizeErr:
      return "IndexSizeError";
    case DOMException::kInvalidCharacterErr:
      return "InvalidCharacterError";
    case DOMException::kNoModificationAllowedErr:
      return "NoModificationAllowedError";
    case DOMException::kNotFoundErr:
      return "NotFoundError";
    case DOMException::kNotSupportedErr:
      return "NotSupportedError";
    case DOMException::kInvalidStateErr:
      return "InvalidStateError";
    case DOMException::kSyntaxErr:
      return "SyntaxError";
    case DOMException::kInvalidAccessErr:
      return "InvalidAccessError";
    case DOMException::kTypeMismatchErr:
      return "TypeMismatchError";
    case DOMException::kSecurityErr:
      return "SecurityError";
    case DOMException::kQuotaExceededErr:
      return "QuotaExceededError";
    case DOMException::kReadOnlyErr:
      return "ReadOnlyError";
    case DOMException::kInvalidPointerIdErr:
      return "InvalidPointerId";
    case DOMException::kNotAllowedErr:
      return "NotAllowedError";
  }
  NOTREACHED();
  return "";
}

// The code attribute's getter must return the legacy code indicated in the
// error names table for this DOMException object's name, or 0 if no such entry
// exists in the table.
//   https://heycam.github.io/webidl/#dom-domexception-code
DOMException::ExceptionCode GetLegacyCodeValue(
    DOMException::ExceptionCode value) {
  return value > DOMException::kHighestErrCodeValue ? DOMException::kNone
                                                    : value;
}
}  // namespace

DOMException::DOMException(ExceptionCode code)
    : code_(GetLegacyCodeValue(code)), name_(GetErrorName(code)) {}

DOMException::DOMException(ExceptionCode code, const std::string& message)
    : code_(GetLegacyCodeValue(code)),
      name_(GetErrorName(code)),
      message_(message) {}

// static
void DOMException::Raise(ExceptionCode code,
                         script::ExceptionState* exception_state) {
  DCHECK(exception_state);
  if (exception_state) {
    exception_state->SetException(make_scoped_refptr(new DOMException(code)));
  }
}

// static
void DOMException::Raise(ExceptionCode code, const std::string& message,
                         script::ExceptionState* exception_state) {
  DCHECK(exception_state);
  if (exception_state) {
    exception_state->SetException(
        make_scoped_refptr(new DOMException(code, message)));
  }
}

}  // namespace dom
}  // namespace cobalt
