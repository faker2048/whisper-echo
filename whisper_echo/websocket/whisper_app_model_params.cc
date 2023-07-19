#include "whisper_echo/websocket/whisper_app_model_params.h"

#include "spdlog/spdlog.h"
#include "whisper.h"
#include "whisper_echo/model/whisper_context.h"

namespace whisper {

void WhisperPrintSegmentCallback(struct whisper_context *,
                                 struct whisper_state *,
                                 int n_new,
                                 void *user_data) {
  WhisperContext *context = static_cast<WhisperContext *>(user_data);
  const int n_segments    = context->GetNumberOfSegments();

  int64_t t0 = 0;
  int64_t t1 = 0;

  // print the last n_new segments
  const int s0 = n_segments - n_new;

  if (s0 == 0) {
    spdlog::info("Segmentation:");
  }

  for (int i = s0; i < n_segments; i++) {
    auto text = context->GetSegmentText(i);
    spdlog::info("{}", text);
  }
}

whisper_full_params GetWhisperFullParams(const WhisperAppModelParams &params,
                                         WhisperContext *ctx) {
  whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  wparams.strategy =
      params.beam_size > 1 ? WHISPER_SAMPLING_BEAM_SEARCH : WHISPER_SAMPLING_GREEDY;
  wparams.print_progress   = params.print_progress;
  wparams.print_timestamps = params.print_timestamps;
  wparams.print_special    = params.print_special;
  wparams.translate        = params.translate;
  wparams.language         = params.language.c_str();
  wparams.detect_language  = params.detect_language;
  wparams.n_threads        = params.n_threads;
  wparams.n_max_text_ctx =
      params.max_context >= 0 ? params.max_context : wparams.n_max_text_ctx;
  wparams.offset_ms   = params.offset_t_ms;
  wparams.duration_ms = params.duration_ms;

  wparams.token_timestamps = params.token_timestamps || params.max_len > 0;
  wparams.thold_pt         = params.word_thold;
  wparams.max_len = params.token_timestamps && params.max_len == 0 ? 60 : params.max_len;
  wparams.split_on_word         = params.split_on_word;
  wparams.speed_up              = params.speed_up;
  wparams.initial_prompt        = params.prompt.c_str();
  wparams.greedy.best_of        = params.best_of;
  wparams.beam_search.beam_size = params.beam_size;

  wparams.temperature_inc = wparams.temperature_inc;
  wparams.entropy_thold   = params.entropy_thold;
  wparams.logprob_thold   = params.logprob_thold;

  if (ctx && params.print_segments) {
    wparams.new_segment_callback           = WhisperPrintSegmentCallback;
    wparams.new_segment_callback_user_data = ctx;
  }

  return wparams;
}

}  // namespace whisper