// TODO: implement whisper_echo.cc

#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"
#include "whisper_echo/websocket/whisper_app.h"
#include "whisper_echo/websocket/whisper_websocket_controller.h"
#include "whisper_echo/model/whisper_context.h"

// set log level debug
void set_log_level() {
  spdlog::set_level(spdlog::level::debug);
  spdlog::cfg::load_env_levels();  // SPDLOG_LEVEL
  spdlog::info(
      "spdlog log level: {}, for setting log level use env variable "
      "SPDLOG_LEVEL=trace/debug/info/warn/error/critical/off",
      spdlog::level::to_string_view(spdlog::get_level()));
}

int main(int argc, char* argv[]) {
  set_log_level();

  std::string model_path;

  if (argc < 2) {
    std::cerr << "Usage: whisper_echo <model_path>" << std::endl;
    return 1;
  }

  model_path = argv[1];

  whisper::WhisperContextSingleton::GetSingletonInstance().Init(model_path);
  whisper::WhisperApp::Builder().Listen("0.0.0.0").SetPort(8008).Build().Run();

  return 0;
}