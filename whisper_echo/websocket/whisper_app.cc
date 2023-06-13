#include "whisper_echo/websocket/whisper_app.h"

#include "spdlog/spdlog.h"
#include "whisper_echo/websocket/whisper_websocket_controller.h"

namespace whisper {

WhisperApp::Config WhisperApp::Config::Default() {
  return Config{
      .host = "127.0.0.1",
      .port = 8008,
  };
}

WhisperApp::Builder::Builder() : config_(Config::Default()) {
}

WhisperApp::Builder::~Builder() {
}

WhisperApp::Builder& WhisperApp::Builder::SetHost(const std::string& host) {
  config_.host = host;
  return *this;
}

WhisperApp::Builder& WhisperApp::Builder::SetPort(int port) {
  config_.port = port;
  return *this;
}

WhisperApp WhisperApp::Builder::Build() {
  return WhisperApp(std::move(config_));
}

WhisperApp::WhisperApp(Config&& config) : config_(std::move(config)) {
}

WhisperApp::WhisperApp(const Config& config) : config_(config) {
}

void WhisperApp::Run() {
  spdlog::info("🛫 Starting whisper app on {}:{}", config_.host, config_.port);
  drogon::app()
      .setClientMaxWebSocketMessageSize(std::numeric_limits<size_t>::max())
      .addListener(config_.host, config_.port)
      .run();
}

}  // namespace whisper