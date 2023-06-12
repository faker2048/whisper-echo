#pragma once

#include <string_view>
#include <vector>

#include "whisper.h"

namespace whisper {

class WhisperContext {
 public:
  static WhisperContext FromFile(const std::string& model_path);

  explicit WhisperContext(struct whisper_context* ctx);

  ~WhisperContext();

  int RunFull(const std::vector<float>& audio_data, whisper_full_params wparams);

  int GetNumberOfSegments();

  std::string_view GetSegmentText(int segment_index);

 private:
  struct whisper_context* ctx;
};

}  // namespace whisper
