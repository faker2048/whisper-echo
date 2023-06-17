// TODO: refactor this file
#define DR_WAV_IMPLEMENTATION

#include <filesystem>
#include <iostream>

#include "spdlog/spdlog.h"
#include "whisper.h"
#include "whisper_echo/examples/cli/dr_wav.h"  // TODO: move to third-party
#include "whisper_echo/whisper_context.h"
#include "whisper_echo/whisper_params.h"

using whisper::WhisperContext;
using whisper::WhisperParams;

constexpr int COMMON_SAMPLE_RATE = 16000;

bool read_wav(const std::string &fname,
              std::vector<float> &pcmf32,
              std::vector<std::vector<float>> &pcmf32s,
              bool stereo) {
  drwav wav;
  std::vector<uint8_t> wav_data;  // used for pipe input from stdin

  if (fname == "-") {
    {
      uint8_t buf[1024];
      while (true) {
        const size_t n = fread(buf, 1, sizeof(buf), stdin);
        if (n == 0) {
          break;
        }
        wav_data.insert(wav_data.end(), buf, buf + n);
      }
    }

    if (drwav_init_memory(&wav, wav_data.data(), wav_data.size(), nullptr) == false) {
      fprintf(stderr, "error: failed to open WAV file from stdin\n");
      return false;
    }

    fprintf(stderr, "%s: read %zu bytes from stdin\n", __func__, wav_data.size());
  } else if (drwav_init_file(&wav, fname.c_str(), nullptr) == false) {
    fprintf(stderr, "error: failed to open '%s' as WAV file\n", fname.c_str());
    return false;
  }

  if (wav.channels != 1 && wav.channels != 2) {
    fprintf(
        stderr, "%s: WAV file '%s' must be mono or stereo\n", __func__, fname.c_str());
    return false;
  }

  if (stereo && wav.channels != 2) {
    fprintf(stderr,
            "%s: WAV file '%s' must be stereo for diarization\n",
            __func__,
            fname.c_str());
    return false;
  }

  if (wav.sampleRate != COMMON_SAMPLE_RATE) {
    fprintf(stderr,
            "%s: WAV file '%s' must be %i kHz\n",
            __func__,
            fname.c_str(),
            COMMON_SAMPLE_RATE / 1000);
    return false;
  }

  if (wav.bitsPerSample != 16) {
    fprintf(stderr, "%s: WAV file '%s' must be 16-bit\n", __func__, fname.c_str());
    return false;
  }

  const uint64_t n = wav_data.empty()
                         ? wav.totalPCMFrameCount
                         : wav_data.size() / (wav.channels * wav.bitsPerSample / 8);

  std::vector<int16_t> pcm16;
  pcm16.resize(n * wav.channels);
  drwav_read_pcm_frames_s16(&wav, n, pcm16.data());
  drwav_uninit(&wav);

  // convert to mono, float
  pcmf32.resize(n);
  if (wav.channels == 1) {
    for (uint64_t i = 0; i < n; i++) {
      pcmf32[i] = float(pcm16[i]) / 32768.0f;
    }
  } else {
    for (uint64_t i = 0; i < n; i++) {
      pcmf32[i] = float(pcm16[2 * i] + pcm16[2 * i + 1]) / 65536.0f;
    }
  }

  if (stereo) {
    // convert to stereo, float
    pcmf32s.resize(2);

    pcmf32s[0].resize(n);
    pcmf32s[1].resize(n);
    for (uint64_t i = 0; i < n; i++) {
      pcmf32s[0][i] = float(pcm16[2 * i]) / 32768.0f;
      pcmf32s[1][i] = float(pcm16[2 * i + 1]) / 32768.0f;
    }
  }

  return true;
}

std::string to_timestamp(int64_t t, bool comma = false) {
  int64_t msec = t * 10;
  int64_t hr   = msec / (1000 * 60 * 60);
  msec         = msec - hr * (1000 * 60 * 60);
  int64_t min  = msec / (1000 * 60);
  msec         = msec - min * (1000 * 60);
  int64_t sec  = msec / 1000;
  msec         = msec - sec * 1000;

  char buf[32];
  snprintf(buf,
           sizeof(buf),
           "%02d:%02d:%02d%s%03d",
           (int)hr,
           (int)min,
           (int)sec,
           comma ? "," : ".",
           (int)msec);

  return std::string(buf);
}

void whisper_print_segment_callback(struct whisper_context *,
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
    std::cout << std::endl;
  }

  for (int i = s0; i < n_segments; i++) {
    auto text = context->GetSegmentText(i);
    std::cout << text << std::endl;
  }
}

whisper_full_params get_whisper_full_params(const WhisperParams &params,
                                            WhisperContext *ctx) {
  whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

  wparams.strategy =
      params.beam_size > 1 ? WHISPER_SAMPLING_BEAM_SEARCH : WHISPER_SAMPLING_GREEDY;

  wparams.print_realtime   = false;
  wparams.print_progress   = params.print_progress;
  wparams.print_timestamps = !params.no_timestamps;
  wparams.print_special    = params.print_special;
  wparams.translate        = params.translate;
  wparams.language         = params.language.c_str();
  wparams.detect_language  = params.detect_language;
  wparams.n_threads        = params.n_threads;
  wparams.n_max_text_ctx =
      params.max_context >= 0 ? params.max_context : wparams.n_max_text_ctx;
  wparams.offset_ms   = params.offset_t_ms;
  wparams.duration_ms = params.duration_ms;

  wparams.token_timestamps = false /* params.output_wts */ || params.max_len > 0;
  wparams.thold_pt         = params.word_thold;
  wparams.max_len =
      false /* params.output_wts */ && params.max_len == 0 ? 60 : params.max_len;
  wparams.split_on_word = params.split_on_word;

  wparams.speed_up = params.speed_up;

  wparams.initial_prompt = params.prompt.c_str();

  wparams.greedy.best_of        = params.best_of;
  wparams.beam_search.beam_size = params.beam_size;

  wparams.temperature_inc = params.no_fallback ? 0.0f : wparams.temperature_inc;
  wparams.entropy_thold   = params.entropy_thold;
  wparams.logprob_thold   = params.logprob_thold;

  // this callback is called on each new segment
  if (!wparams.print_realtime) {
    wparams.new_segment_callback           = whisper_print_segment_callback;
    wparams.new_segment_callback_user_data = ctx;
  }

  // example for abort mechanism
  // in this example, we do not abort the processing, but we could if the flag is set to
  // true the callback is called before every encoder run - if it returns false, the
  // processing is aborted
  {
    static bool is_aborted = false;  // NOTE: this should be atomic to avoid data race

    wparams.encoder_begin_callback = [](struct whisper_context * /*ctx*/,
                                        struct whisper_state * /*state*/,
                                        void *user_data) {
      bool is_aborted = *(bool *)user_data;
      return !is_aborted;
    };
    wparams.encoder_begin_callback_user_data = &is_aborted;
  }
  return wparams;
}

void check_files(const WhisperParams &params) {
  if (params.fname_inp.empty()) {
    params.PrintUsage();
    spdlog::critical("no input file specified");
    exit(1);
  } else {
    for (const auto &fname : params.fname_inp) {
      if (!std::filesystem::exists(fname)) {
        spdlog::critical("input file '{}' does not exist", fname);
        exit(1);
      }
    }
  }

  if (params.model.empty() || !std::filesystem::exists(params.model)) {
    spdlog::critical("no model file specified");
    exit(1);
  }
}

int main(int argc, char **argv) {
  WhisperParams params(argc, argv);
  check_files(params);

  WhisperContext context(params.model);
  auto wparams = get_whisper_full_params(params, &context);

  std::vector<float> pcmf32;                // mono-channel F32 PCM
  std::vector<std::vector<float>> pcmf32s;  // stereo-channel F32 PCM
  if (!::read_wav(params.fname_inp.front(), pcmf32, pcmf32s, params.diarize)) {
    spdlog::critical("error: failed to read WAV file '{}'", params.fname_inp.front());
    exit(1);
  }

  context.RunFull(pcmf32, wparams);

  return 0;
}
