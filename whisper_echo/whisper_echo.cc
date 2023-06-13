// TODO: implement whisper_echo.cc

#include "spdlog/spdlog.h"
#include "whisper_echo/websocket/whisper_app.h"
#include "whisper_echo/websocket/whisper_websocket_controller.h"

int main() {
  auto whisper_app =
      whisper::WhisperApp::Builder().SetHost("127.0.0.1").SetPort(8008).Build();
  whisper_app.Run();
}