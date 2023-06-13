add_executable(main main.cc)
target_link_libraries(main PRIVATE whisper whisper_params whisper_context)
set_output_dir(main examples)
