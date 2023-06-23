#include "whisper_echo/whisper_context.h"

#include <iostream>

#include "spdlog/spdlog.h"

namespace whisper {

WhisperContext::WhisperContext(const std::string& model_path) {
  ctx_ = whisper_init_from_file(model_path.c_str());
  if (ctx_ == nullptr) {
    spdlog::critical("Failed to initialize whisper context");
    exit(1);
  }
}

WhisperContext::WhisperContext(whisper_context* ctx) : ctx_(ctx) {
}

WhisperContext::~WhisperContext() {
  whisper_free(ctx_);
}

int WhisperContext::RunFull(const std::vector<float>& audio_data,
                            whisper_full_params wparams) {
  int ret = whisper_full(ctx_, wparams, audio_data.data(), audio_data.size());
  if (ret != 0) {
    spdlog::critical("Failed to run whisper_full");
    exit(1);
  }
  return ret;
}

int WhisperContext::GetNumberOfSegments() const {
  return whisper_full_n_segments(ctx_);
}

std::string_view WhisperContext::GetSegmentText(int segment_index) const {
  const char* text = whisper_full_get_segment_text(ctx_, segment_index);
  return std::string_view(text);
}

}  // namespace whisper
