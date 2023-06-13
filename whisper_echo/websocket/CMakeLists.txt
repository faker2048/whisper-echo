add_library(whisper_websocket_controller OBJECT whisper_websocket_controller.cc)
target_link_libraries(whisper_websocket_controller PUBLIC spdlog::spdlog
                                                          Drogon::Drogon)

add_library(whisper_app OBJECT whisper_app.cc)
target_link_libraries(whisper_app PUBLIC whisper_websocket_controller
                                         spdlog::spdlog)
