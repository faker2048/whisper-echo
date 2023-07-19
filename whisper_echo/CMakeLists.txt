add_cmake_subdirectories()

add_executable(whisper_echo whisper_echo.cc)
target_link_libraries(whisper_echo PUBLIC whisper_app
                                          whisper_websocket_controller)
set_output_dir(whisper_echo bin)
