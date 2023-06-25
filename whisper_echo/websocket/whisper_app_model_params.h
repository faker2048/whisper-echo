#pragma once

#include <string>
#include <thread>
#include <vector>

// forward declaration for reduce compilation time
class whisper_full_params;

namespace whisper {

class WhisperContext;

struct WhisperAppModelParams {
  int32_t n_threads   = std::min(4, (int32_t)std::thread::hardware_concurrency());
  int32_t offset_t_ms = 0;   // start offset in ms
  int32_t duration_ms = 0;   // audio duration to process in ms
  int32_t max_context = -1;  // max tokens to use from past text as prompt for the decoder
  int32_t max_len     = 0;   // max segment length in characters
  int32_t best_of   = 5;  // number of candidates when sampling with non-zero temperature
  int32_t beam_size = 5;  // number of beams in beam search, only applicable when
                          // temperature is zero

  float temperature     = 0.0f;    // sampling temperature
  float word_thold      = 0.01f;   // timestamp token probability threshold (~0.01)
  float entropy_thold   = 2.40f;   // similar to OpenAI's "compression_ratio_threshold"
  float logprob_thold   = -1.00f;  // log probability threshold for decoder fail
  float temperature_inc = 0.2f;    // temperature to increase when falling back when the
                                 // decoding fails to meet either of the thresholds below

  bool token_timestamps = false;  // timestamps for each token
  bool speed_up         = false;  // [EXPERIMENTAL] speed-up techniques
                          // note: these can significantly reduce the quality of the
                          // output speed-up the audio by 2x using Phase Vocoder
  bool translate       = false;  // translate the text to English
  bool detect_language = false;
  bool split_on_word =
      false;  // split on word rather than on token (when used with max_len)

  bool print_segments = false;  // log the segments text
  bool print_special  = false;  // print special tokens (e.g. <SOT>, <EOT>, <BEG>, etc.)
  bool print_progress = false;  // print progress information
  bool print_timestamps =
      false;  // print timestamps for each text segment when printing realtime

  std::string language = "en";  // for auto-detection, set to nullptr, "" or "auto"

  std::string prompt =
      {};  // tokens to provide to the whisper decoder as initial prompt these
           // are prepended to any existing text context from a previous call

  inline static WhisperAppModelParams Defualt() {
    return WhisperAppModelParams();
  }
};

/*
 * @brief Get the whisper full params object
 *
 * @param params WhisperAppModelParams
 * @param ctx_for_print WhisperContext, set it when print_segments is true, default is
 * nullptr. if not set, then will not print segments text when running whisper_full
 * @return whisper_full_params whisper_full_params that can be used in whisper_full
 */
whisper_full_params GetWhisperFullParams(const WhisperAppModelParams &params,
                                         WhisperContext *ctx_for_print = nullptr);

}  // namespace whisper