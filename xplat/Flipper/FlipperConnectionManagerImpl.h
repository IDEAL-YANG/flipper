/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#pragma once

#include "FlipperInitConfig.h"
#include "FlipperConnectionManager.h"
#include "FlipperState.h"
#include <folly/Executor.h>
#include <folly/io/async/EventBase.h>
#include <rsocket/RSocket.h>
#include <mutex>

namespace facebook {
namespace flipper {

class ConnectionEvents;
class ConnectionContextStore;
class Responder;

class FlipperConnectionManagerImpl : public FlipperConnectionManager {
  friend ConnectionEvents;
  friend Responder;

 public:
  FlipperConnectionManagerImpl(FlipperInitConfig config, std::shared_ptr<FlipperState> state, std::shared_ptr<ConnectionContextStore> contextStore);

  ~FlipperConnectionManagerImpl();

  void start() override;

  void stop() override;

  bool isOpen() const override;

  void setCallbacks(Callbacks* callbacks) override;

  void sendMessage(const folly::dynamic& message) override;

  void reconnect();

 private:
  bool isOpen_ = false;
  Callbacks* callbacks_;
  DeviceData deviceData_;
  std::shared_ptr<FlipperState> flipperState_;
  int insecurePort;
  int securePort;

  folly::EventBase* flipperEventBase_;
  folly::EventBase* connectionEventBase_;
  std::unique_ptr<rsocket::RSocketClient> client_;
  bool connectionIsTrusted_;
  int failedConnectionAttempts_ = 0;
  std::shared_ptr<ConnectionContextStore> contextStore_;

  void startSync();
  void doCertificateExchange();
  void connectSecurely();
  bool isCertificateExchangeNeeded();
  void requestSignedCertFromFlipper();
  bool isRunningInOwnThread();
  void sendLegacyCertificateRequest(folly::dynamic message);
  std::string getDeviceId();
};

} // namespace flipper
} // namespace facebook
