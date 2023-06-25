#pragma once

#include <string_view>
#include <vector>

#include "whisper.h"
#include "whisper_echo/utlis/singleton.h"

namespace whisper {

class WhisperContext {
 public:
  struct Segment {
    std::string_view text;
    int64_t start_time;
    int64_t end_time;
  };
  explicit WhisperContext(struct whisper_context* ctx);
  WhisperContext(const std::string& model_path);

  WhisperContext(const WhisperContext&)            = delete;
  WhisperContext& operator=(const WhisperContext&) = delete;

  ~WhisperContext();

  int RunFull(const std::vector<float>& audio_data, whisper_full_params wparams);

  int GetNumberOfSegments() const;

  std::string_view GetSegmentText(int segment_index) const;

  std::vector<Segment> GetSegments() const;

  std::string GetFullText() const;

 private:
  struct whisper_context* ctx_ = nullptr;
};

using WhisperContextSingleton = Singleton<WhisperContext>;

}  // namespace whisper
