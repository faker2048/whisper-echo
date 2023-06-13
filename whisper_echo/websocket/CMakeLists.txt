add_library(whisper_websocket_controller OBJECT whisper_websocket_controller.cc)
target_link_libraries(whisper_websocket_controller PUBLIC spdlog::spdlog)

add_library(whisper_app OBJECT whisper_app.cc)
target_link_libraries(whisper_app PUBLIC spdlog::spdlog)
