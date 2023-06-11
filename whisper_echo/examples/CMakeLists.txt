
add_library(whisper_params OBJECT whisper_params.cc)

add_executable(main main.cc)
target_link_libraries(main PRIVATE whisper whisper_params)
