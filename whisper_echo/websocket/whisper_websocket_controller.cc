#include "whisper_echo/websocket/whisper_websocket_controller.h"

#include <chrono>
#include <memory>
#include <span>

#include "spdlog/spdlog.h"
#include "whisper_echo/utlis/id_generator.h"

namespace whisper {

namespace {

int64_t Now() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}  // namespace

constexpr int kSimpleRate = 16000;

enum class MessageType {
  Unknown,
  Start,
  End,
  Data,
};

MessageType FromString(std::string_view str) {
  if (str == "start") {
    return MessageType::Start;
  } else if (str == "end") {
    return MessageType::End;
  } else if (str == "data") {
    return MessageType::Data;
  } else {
    return MessageType::Unknown;
  }
}

struct MessageMetadata {
  MessageType type;
  std::string original_str;
  std::span<const float> audio_float_vector;
  int sequence_number;

  // "type_str|seq_str|binary_audiodata"
  static MessageMetadata FromBinaryString(std::string &&str) {
    MessageMetadata ret{
        .type         = MessageType::Unknown,
        .original_str = std::move(str),
    };

    size_t i                  = ret.original_str.find_first_of('|');
    std::string_view type_str = std::string_view{ret.original_str.data(), i};
    ret.type                  = FromString(type_str);
    if (ret.type != MessageType::Data) {
      return ret;
    }

    size_t j = ret.original_str.find_first_of('|', i + 1);
    std::string_view seq_str =
        std::string_view{ret.original_str.data() + i + 1, j - i - 1};
    ret.sequence_number = std::stoi(seq_str.data());

    std::string_view data_str = std::string_view{ret.original_str.data() + j + 1,
                                                 ret.original_str.size() - j - 1};

    if (ret.type == MessageType::Data) {
      ret.audio_float_vector =
          std::span<const float>{reinterpret_cast<const float *>(data_str.data()),
                                 data_str.size() / sizeof(float)};
    }
    return ret;
  }
};

class ConnectionContext {
 public:
  ConnectionContext(std::string peer_addr)
      : peer_addr_(std::move(peer_addr)),
        session_id_(TimestampIDGenerator::GenerateId()) {
  }

  void AppendAudioData(std::span<const float> audio_data) {
    audio_data_.insert(audio_data_.end(), audio_data.begin(), audio_data.end());
  }

  inline std::string peer_addr() const {
    return peer_addr_;
  }

  inline int64_t session_id() const {
    return session_id_;
  }

 private:
  std::string peer_addr_;
  int64_t session_id_;
  std::vector<float> audio_data_;
};

void WhisperWebSocketController::handleNewMessage(
    const drogon::WebSocketConnectionPtr &ws_conn,
    std::string &&message,
    const drogon::WebSocketMessageType &type) {
  MessageMetadata message_obj = MessageMetadata::FromBinaryString(std::move(message));
  auto conn_ctx               = ws_conn->getContext<ConnectionContext>();

  if (message_obj.type == MessageType::Start) {
    // TODO: Start a new session.
    ws_conn->send("start confirmed");
  } else if (message_obj.type == MessageType::End) {
    ws_conn->send("end confirmed");
  } else if (message_obj.type == MessageType::Data) {
    conn_ctx->AppendAudioData(message_obj.audio_float_vector);
    ws_conn->send(fmt::format("data confirmed|seq {}", message_obj.sequence_number));
  }
}

void WhisperWebSocketController::handleConnectionClosed(
    const drogon::WebSocketConnectionPtr &conn) {
  auto conn_ctx = conn->getContext<ConnectionContext>();
  spdlog::debug("WebSocket closed by {}",
                conn_ctx ? conn_ctx->peer_addr() : "unknown peer");
}

void WhisperWebSocketController::handleNewConnection(
    const drogon::HttpRequestPtr &req, const drogon::WebSocketConnectionPtr &conn) {
  spdlog::debug("New WebSocket connection!");

  auto conn_ctx = std::make_shared<ConnectionContext>(conn->peerAddr().toIpPort());
  conn->setContext(conn_ctx);
}

}  // namespace whisper
