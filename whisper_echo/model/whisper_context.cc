#include "whisper_echo/model/whisper_context.h"

#include <iostream>

#include "spdlog/spdlog.h"
#include "whisper_echo/utlis/scope_timer.h"

namespace whisper {

using ScoopTimer = utlis::ScopeTimer<[](std::string_view s) { spdlog::info(s); }>;

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
  ScoopTimer timer("WhisperContext::RunFull");

  spdlog::info("Running whisper_full, audio_data.size() = {}", audio_data.size());
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

std::vector<WhisperContext::Segment> WhisperContext::GetSegments() const {
  std::vector<Segment> segments;
  const int n_segments = whisper_full_n_segments(ctx_);
  segments.reserve(n_segments);
  for (int i = 0; i < n_segments; i++) {
    const char* text   = whisper_full_get_segment_text(ctx_, i);
    int64_t start_time = whisper_full_get_segment_t0(ctx_, i);
    int64_t end_time   = whisper_full_get_segment_t1(ctx_, i);
    segments.push_back({std::string_view(text), start_time, end_time});
  }
  return segments;
}

std::string WhisperContext::GetFullText() const {
  std::string text;
  for (const auto& segment : GetSegments()) {
    text += segment.text;
  }
  return text;
}

}  // namespace whisper
