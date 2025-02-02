// Copyright 2015 The Cobalt Authors. All Rights Reserved.
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

#include "starboard/socket.h"

#include <errno.h>
#include <sys/socket.h>

#include "starboard/log.h"
#include "starboard/shared/posix/handle_eintr.h"
#include "starboard/shared/posix/socket_internal.h"

bool SbSocketIsConnectedAndIdle(SbSocket socket) {
  if (!SbSocketIsValid(socket)) {
    return false;
  }

  SB_DCHECK(socket->socket_fd >= 0);

  // To tell if it is really connected and idle, we peek a byte from the
  // stream. We should get an EAGAIN/EWOULDBLOCK telling us the socket is
  // waiting for data.
  char c;
  int rv = HANDLE_EINTR(recv(socket->socket_fd, &c, 1, MSG_PEEK));
  if (rv >= 0) {
    // Either not connected, or not idle.
    return false;
  }

  return (errno == EAGAIN || errno == EWOULDBLOCK);
}
