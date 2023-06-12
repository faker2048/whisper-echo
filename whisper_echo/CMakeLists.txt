add_subdirectory(examples)

add_library(whisper_params OBJECT whisper_params.cc)

add_library(whisper_context OBJECT whisper_context.cc)
target_link_libraries(whisper_context PUBLIC whisper)
