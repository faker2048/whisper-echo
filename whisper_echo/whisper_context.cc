#include "whisper_echo/whisper_context.h"

#include <iostream>

#include "spdlog/spdlog.h"

namespace whisper {

WhisperContext WhisperContext::FromFile(const std::string& model_path) {
  whisper_context* ctx = whisper_init_from_file(model_path.c_str());
  if (ctx == nullptr) {
    spdlog::error("Failed to initialize whisper context");
    exit(1);
  }
  return WhisperContext(ctx);
}

WhisperContext::WhisperContext(whisper_context* ctx) : ctx(ctx) {
}

WhisperContext::~WhisperContext() {
  whisper_free(ctx);
}

int WhisperContext::RunFull(const std::vector<float>& audio_data,
                            whisper_full_params wparams) {
  int ret = whisper_full(ctx, wparams, audio_data.data(), audio_data.size());
  if (ret != 0) {
    spdlog::error("Failed to run whisper_full");
    exit(1);
  }
  return ret;
}

int WhisperContext::GetNumberOfSegments() const {
  return whisper_full_n_segments(ctx);
}

std::string_view WhisperContext::GetSegmentText(int segment_index) const {
  const char* text = whisper_full_get_segment_text(ctx, segment_index);
  return std::string_view(text);
}

}  // namespace whisper
