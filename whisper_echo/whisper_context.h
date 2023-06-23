#pragma once

#include <string_view>
#include <vector>

#include "whisper.h"
#include "whisper_echo/utlis/singleton.h"

namespace whisper {

class WhisperContext {
 public:
  explicit WhisperContext(struct whisper_context* ctx);
  WhisperContext(const std::string& model_path);

  WhisperContext(const WhisperContext&)            = delete;
  WhisperContext& operator=(const WhisperContext&) = delete;

  ~WhisperContext();

  int RunFull(const std::vector<float>& audio_data, whisper_full_params wparams);

  int GetNumberOfSegments() const;

  std::string_view GetSegmentText(int segment_index) const;

 private:
  struct whisper_context* ctx_ = nullptr;
};

using WhisperContextSingleton = Singleton<WhisperContext>;

}  // namespace whisper
