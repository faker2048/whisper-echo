add_executable(main main.cc)
target_link_libraries(main PRIVATE whisper whisper_command_line_params
                                   whisper_context)
set_output_dir(main examples)

add_library(whisper_command_line_params STATIC whisper_command_line_params.cc)
target_link_libraries(whisper_command_line_params PRIVATE spdlog::spdlog)
