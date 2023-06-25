add_library(whisper_app_model_params STATIC whisper_app_model_params.cc)
target_link_libraries(whisper_app_model_params PUBLIC whisper_context
                                                      spdlog::spdlog)

add_library(whisper_websocket_controller STATIC whisper_websocket_controller.cc)
target_link_libraries(
  whisper_websocket_controller
  PUBLIC spdlog::spdlog Drogon::Drogon id_generator whisper_context msgpack-cxx
         whisper_app_model_params)

add_library(whisper_app STATIC whisper_app.cc)
target_link_libraries(whisper_app PUBLIC whisper_websocket_controller
                                         spdlog::spdlog)
