// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NET_SOCKET_STREAM_SOCKET_STREAM_JOB_H_
#define NET_SOCKET_STREAM_SOCKET_STREAM_JOB_H_

#include <string>

#include "base/memory/ref_counted.h"
#include "net/base/net_export.h"
#include "net/socket_stream/socket_stream.h"

class GURL;

namespace net {

class SSLConfigService;
class SSLInfo;
class TransportSecurityState;

// SocketStreamJob represents full-duplex communication over SocketStream.
// If a protocol (e.g. WebSocket protocol) needs to inspect/modify data
// over SocketStream, you can implement protocol specific job (e.g.
// WebSocketJob) to do some work on data over SocketStream.
// Registers the protocol specific SocketStreamJob by RegisterProtocolFactory
// and call CreateSocketStreamJob to create SocketStreamJob for the URL.
class NET_EXPORT SocketStreamJob
    : public base::RefCountedThreadSafe<SocketStreamJob> {
 public:
  // Callback function implemented by protocol handlers to create new jobs.
  typedef SocketStreamJob* (ProtocolFactory)(const GURL& url,
                                             SocketStream::Delegate* delegate);

  static ProtocolFactory* RegisterProtocolFactory(const std::string& scheme,
                                                  ProtocolFactory* factory);

  static SocketStreamJob* CreateSocketStreamJob(
      const GURL& url,
      SocketStream::Delegate* delegate,
      TransportSecurityState* sts,
      SSLConfigService* ssl);

  SocketStreamJob();
  void InitSocketStream(SocketStream* socket) {
    socket_ = socket;
  }

  virtual SocketStream::UserData* GetUserData(const void* key) const;
  virtual void SetUserData(const void* key, SocketStream::UserData* data);

  const URLRequestContext* context() const {
    return socket_->context();
  }
  void set_context(const URLRequestContext* context) {
    socket_->set_context(context);
  }
  void set_network_task_runner(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner) {
    network_task_runner_ = task_runner;
  }

  base::SingleThreadTaskRunner* network_task_runner() {
    return network_task_runner_.get();
  }

  virtual void Connect();

  virtual bool SendData(const char* data, int len);

  virtual void Close();

  virtual void RestartWithAuth(const AuthCredentials& credentials);

  virtual void CancelWithError(int error);

  virtual void CancelWithSSLError(const net::SSLInfo& ssl_info);

  virtual void ContinueDespiteError();

  virtual void DetachDelegate();

 protected:
  friend class WebSocketJobSpdy2Test;
  friend class WebSocketJobSpdy3Test;
  friend class base::RefCountedThreadSafe<SocketStreamJob>;
  virtual ~SocketStreamJob();

  scoped_refptr<SocketStream> socket_;
  scoped_refptr<base::SingleThreadTaskRunner> network_task_runner_;

  DISALLOW_COPY_AND_ASSIGN(SocketStreamJob);
};

}  // namespace net

#endif  // NET_SOCKET_STREAM_SOCKET_STREAM_JOB_H_
