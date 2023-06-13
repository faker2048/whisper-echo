#include "whisper_echo/websocket/whisper_websocket_controller.h"

#include "spdlog/spdlog.h"

namespace whisper {

void WhisperWebSocketController::handleNewMessage(
    const drogon::WebSocketConnectionPtr &ws_conn_ptr,
    std::string &&message,
    const drogon::WebSocketMessageType &type) {
  // Write your application logic here
  spdlog::debug("New WebSocket message size: {}", message.size());
  spdlog::trace("New WebSocket message: {}", message);

  if (type == drogon::WebSocketMessageType::Ping) {
    spdlog::debug("Received a ping");
  } else if (type == drogon::WebSocketMessageType::Text) {
    spdlog::debug("Received a text message");
  } else if (type == drogon::WebSocketMessageType::Binary) {
    spdlog::debug("Received a binary message");
  }
}

void WhisperWebSocketController::handleConnectionClosed(
    const drogon::WebSocketConnectionPtr &conn) {
  spdlog::debug("WebSocket closed!");
}

void WhisperWebSocketController::handleNewConnection(
    const drogon::HttpRequestPtr &req, const drogon::WebSocketConnectionPtr &conn) {
  spdlog::debug("New WebSocket connection!");
}

}  // namespace whisper