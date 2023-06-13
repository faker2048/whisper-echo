#include <drogon/HttpAppFramework.h>
#include <drogon/PubSubService.h>
#include <drogon/WebSocketController.h>

using namespace drogon;

class WebSocketChat : public drogon::WebSocketController<WebSocketChat> {
 public:
  void handleNewMessage(const WebSocketConnectionPtr &,
                        std::string &&,
                        const WebSocketMessageType &) override;
  void handleConnectionClosed(const WebSocketConnectionPtr &) override;
  void handleNewConnection(const HttpRequestPtr &,
                           const WebSocketConnectionPtr &) override;

  WS_PATH_LIST_BEGIN
  WS_PATH_ADD("/chat", Get);
  WS_PATH_LIST_END

 private:
  PubSubService<std::string> chat_rooms_;
};

struct Subscriber {
  std::string chat_room_name_;
  drogon::SubscriberID id_;
};

void WebSocketChat::handleNewMessage(const WebSocketConnectionPtr &ws_conn_ptr,
                                     std::string &&message,
                                     const WebSocketMessageType &type) {
  // Write your application logic here
  LOG_DEBUG << "New WebSocket message: " << message;
  if (type == WebSocketMessageType::Ping) {
    LOG_DEBUG << "Received a ping";
  } else if (type == WebSocketMessageType::Text) {
    auto &s = ws_conn_ptr->getContextRef<Subscriber>();
    chat_rooms_.publish(s.chat_room_name_, message);
  }
}

void WebSocketChat::handleConnectionClosed(const WebSocketConnectionPtr &conn) {
  LOG_DEBUG << "WebSocket closed!";
  auto &s = conn->getContextRef<Subscriber>();
  chat_rooms_.unsubscribe(s.chat_room_name_, s.id_);
}

void WebSocketChat::handleNewConnection(const HttpRequestPtr &req,
                                        const WebSocketConnectionPtr &conn) {
  LOG_DEBUG << "New WebSocket connection!";
  conn->send("haha!!!");
  Subscriber s;
  s.chat_room_name_ = req->getParameter("room_name");
  s.id_             = chat_rooms_.subscribe(
      s.chat_room_name_, [conn](const std::string &topic, const std::string &message) {
        // Suppress unused variable warning
        (void)topic;
        conn->send(message);
      });
  conn->setContext(std::make_shared<Subscriber>(std::move(s)));
}

int main() {
  app().addListener("127.0.0.1", 8848).run();
}
