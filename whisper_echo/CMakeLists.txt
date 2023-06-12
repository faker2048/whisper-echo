add_subdirectory(examples)
add_subdirectory(websocket)

add_library(whisper_params OBJECT whisper_params.cc)
target_link_libraries(whisper_params PRIVATE spdlog::spdlog)

add_library(whisper_context OBJECT whisper_context.cc)
target_link_libraries(whisper_context PUBLIC whisper spdlog::spdlog)
