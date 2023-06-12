#pragma once

#include <string>
#include <vector>

namespace whisper {

class WhisperParams {
 public:
  WhisperParams(int argc, char** argv);

  int32_t n_threads;
  int32_t n_processors;
  int32_t offset_t_ms;
  int32_t offset_n;
  int32_t duration_ms;
  int32_t max_context;
  int32_t max_len;
  int32_t best_of;
  int32_t beam_size;

  float word_thold;
  float entropy_thold;
  float logprob_thold;

  bool speed_up;
  bool translate;
  bool detect_language;
  bool diarize;
  bool split_on_word;
  bool no_fallback;
  bool output_srt;
  bool print_special;
  bool print_colors;
  bool print_progress;
  bool no_timestamps;

  std::string language;
  std::string prompt;
  std::string font_path;
  std::string model;

  std::vector<std::string> fname_inp;
  std::vector<std::string> fname_out;

  std::vector<std::string> argvs;

  void PrintUsage();

 private:
  bool Parse();
};

}  // namespace whisper