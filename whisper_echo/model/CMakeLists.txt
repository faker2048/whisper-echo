add_library(whisper_context STATIC whisper_context.cc)
target_link_libraries(whisper_context PUBLIC whisper spdlog::spdlog)
