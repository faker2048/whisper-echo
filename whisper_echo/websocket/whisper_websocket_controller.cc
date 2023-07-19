#include "whisper_echo/websocket/whisper_websocket_controller.h"

#include <chrono>
#include <memory>
#include <span>

#include "msgpack.hpp"
#include "spdlog/spdlog.h"
#include "whisper_echo/utlis/id_generator.h"
#include "whisper_echo/websocket/whisper_app_model_params.h"
#include "whisper_echo/model/whisper_context.h"

namespace whisper {

using drogon::WebSocketConnectionPtr;
using drogon::WebSocketMessageType;

constexpr int kSimpleRate = 16000;

class MessageMetadata {
 public:
  std::string type_str;           // "start", "end", "data" TODO: use enum
  int sequence_number;            // 0, 1, 2, 3, ...
  std::vector<float> audio_data;  // 16000 samples per second

  // Decode function
  void decode(msgpack::object const &o) {
    if (o.type != msgpack::type::MAP) throw msgpack::type_error();

    for (size_t i = 0; i < o.via.map.size; ++i) {
      msgpack::object_kv &kv = o.via.map.ptr[i];
      std::string key        = kv.key.as<std::string>();
      if (key == "type_str") {
        type_str = kv.val.as<std::string>();
      } else if (key == "sequence_number") {
        sequence_number = kv.val.as<int>();
      } else if (key == "audio_data") {
        const auto &raw_data = kv.val.as<std::vector<char>>();
        audio_data.resize(raw_data.size() / sizeof(float));
        std::memcpy(audio_data.data(), raw_data.data(), raw_data.size());
      }
    }
  }
};

struct MessageResponse {
  std::string type_str = "unkown";  // "start", "end", "data" TODO: use enum
  int sequence_number  = -1;        // 0, 1, 2, 3, ...
  bool success         = false;
  std::string message  = "";
  std::vector<std::tuple<int64_t, int64_t, std::string>> results;

  MSGPACK_DEFINE_MAP(type_str, sequence_number, success, message, results);
};

template <typename T>
void SendObject(const WebSocketConnectionPtr &ws_conn, const T &obj) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, obj);
  ws_conn->send(sbuf.data(), sbuf.size(), WebSocketMessageType::Binary);
}

struct ConnectionContext {
  std::string peer_addr;
  int64_t session_id;
  std::vector<float> audio_data;

  ConnectionContext(std::string peer_addr)
      : peer_addr(std::move(peer_addr)), session_id(TimestampIDGenerator::GenerateId()) {
  }

  void AppendAudioData(std::span<const float> seg) {
    audio_data.insert(audio_data.end(), seg.begin(), seg.end());
  }

  void reset() {
    audio_data.clear();
  }

  ~ConnectionContext() {
    spdlog::info("ConnectionContext {} destroyed", session_id);
  }
};

void WhisperWebSocketController::handleNewMessage(const WebSocketConnectionPtr &ws_conn,
                                                  std::string &&message,
                                                  const WebSocketMessageType &type) {
  spdlog::info("handleNewMessage, message size: {} Byte", message.size());
  MessageMetadata message_obj;

  try {
    msgpack::object_handle oh    = msgpack::unpack(message.data(), message.size());
    msgpack::object deserialized = oh.get();
    std::cout << "deserialized: " << deserialized << std::endl;
    message_obj.decode(deserialized);
  } catch (const std::exception &e) {
    ws_conn->send(fmt::format("Failed to parse message, e.what(): {}", e.what()));
    ws_conn->forceClose();
    return;
  }
  auto conn_ctx = ws_conn->getContext<ConnectionContext>();

  MessageResponse response;
  response.type_str = message_obj.type_str;
  response.success  = true;

  if (message_obj.type_str == "start") {
    conn_ctx->reset();
  } else if (message_obj.type_str == "end") {
    WhisperAppModelParams params = WhisperAppModelParams::Defualt();
    auto full_params             = GetWhisperFullParams(params);
    auto whisper_context = WhisperContextSingleton::GetSingletonInstance().Instance();
    whisper_context->RunFull(conn_ctx->audio_data, full_params);  // TODO: use real params
    auto segments = whisper_context->GetSegments();
    for (const auto &seg : segments) {
      response.results.emplace_back(seg.start_time, seg.end_time, seg.text);
    }
    conn_ctx->reset();
  } else if (message_obj.type_str == "data") {
    auto info =
        fmt::format("Received audio data, sequence_number: {}, audio_len: {:.3f} s",
                    message_obj.sequence_number,
                    1.0 * message_obj.audio_data.size() / kSimpleRate);
    spdlog::debug(info);
    conn_ctx->AppendAudioData(message_obj.audio_data);
    response.sequence_number = message_obj.sequence_number;
    response.message         = info;
  } else {
    response.success = false;
    response.message = "unknown message type";
  }

  SendObject(ws_conn, response);
}

void WhisperWebSocketController::handleConnectionClosed(
    const WebSocketConnectionPtr &conn) {
  auto conn_ctx = conn->getContext<ConnectionContext>();
  spdlog::debug("WebSocket closed by {}",
                conn_ctx ? conn_ctx->peer_addr : "unknown peer");
}

void WhisperWebSocketController::handleNewConnection(const drogon::HttpRequestPtr &req,
                                                     const WebSocketConnectionPtr &conn) {
  spdlog::debug("New WebSocket connection!");

  auto conn_ctx = std::make_shared<ConnectionContext>(conn->peerAddr().toIpPort());
  conn->setContext(conn_ctx);
}

}  // namespace whisper
