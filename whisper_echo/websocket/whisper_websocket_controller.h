#pragma once

#include "drogon/HttpAppFramework.h"
#include "drogon/WebSocketController.h"

namespace whisper {

class WhisperWebSocketController
    : public drogon::WebSocketController<WhisperWebSocketController> {
 public:
  void handleNewMessage(const drogon::WebSocketConnectionPtr &,
                        std::string &&,
                        const drogon::WebSocketMessageType &) override;
  void handleConnectionClosed(const drogon::WebSocketConnectionPtr &) override;
  void handleNewConnection(const drogon::HttpRequestPtr &,
                           const drogon::WebSocketConnectionPtr &) override;

  WS_PATH_LIST_BEGIN
  WS_PATH_ADD("/whisper", drogon::Get);
  WS_PATH_LIST_END
 private:
};

}  // namespace whisper