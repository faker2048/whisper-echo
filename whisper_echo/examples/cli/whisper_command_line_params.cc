#include "whisper_echo/examples/cli/whisper_command_line_params.h"

#include <thread>

#include "spdlog/spdlog.h"

namespace whisper {

WhisperCommandLineParams::WhisperCommandLineParams(int argc, char** argv)
    : n_threads(std::min(4, (int32_t)std::thread::hardware_concurrency())),
      n_processors(1),
      offset_t_ms(0),
      offset_n(0),
      duration_ms(0),
      max_context(-1),
      max_len(0),
      best_of(2),
      beam_size(-1),
      word_thold(0.01f),
      entropy_thold(2.40f),
      logprob_thold(-1.00f),
      speed_up(false),
      translate(false),
      detect_language(false),
      diarize(false),
      split_on_word(false),
      no_fallback(false),
      output_srt(false),
      print_special(false),
      print_colors(false),
      print_progress(false),
      no_timestamps(false),
      language("en"),
      font_path("/System/Library/Fonts/Supplemental/Courier New Bold.ttf"),
      model("models/ggml-base.en.bin"),
      fname_inp({}),
      fname_out({}) {
  for (int i = 0; i < argc; ++i) {
    argvs.emplace_back(std::string(argv[i]));
  }
  if (!Parse()) {
    spdlog::error("Failed to parse command line arguments");
    exit(1);
  }
}

void WhisperCommandLineParams::PrintUsage() const {
  // clang-format off
  fprintf(stderr, "\n");
  fprintf(stderr, "usage: %s [options] file0.wav file1.wav ...\n", argvs[0].c_str());
  fprintf(stderr, "\n");
  fprintf(stderr, "options:\n");
  fprintf(stderr, "  -h,        --help              [default] show this help message and exit\n");
  fprintf(stderr, "  -t N,      --threads N         [%-7d] number of threads to use during computation\n",    n_threads);
  fprintf(stderr, "  -p N,      --processors N      [%-7d] number of processors to use during computation\n", n_processors);
  fprintf(stderr, "  -ot N,     --offset-t N        [%-7d] time offset in milliseconds\n",                    offset_t_ms);
  fprintf(stderr, "  -on N,     --offset-n N        [%-7d] segment index offset\n",                           offset_n);
  fprintf(stderr, "  -d  N,     --duration N        [%-7d] duration of audio to process in milliseconds\n",   duration_ms);
  fprintf(stderr, "  -mc N,     --max-context N     [%-7d] maximum number of text context tokens to store\n", max_context);
  fprintf(stderr, "  -ml N,     --max-len N         [%-7d] maximum segment length in characters\n",           max_len);
  fprintf(stderr, "  -sow,      --split-on-word     [%-7s] split on word rather than on token\n",             split_on_word ? "true" : "false");
  fprintf(stderr, "  -bo N,     --best-of N         [%-7d] number of best candidates to keep\n",              best_of);
  fprintf(stderr, "  -bs N,     --beam-size N       [%-7d] beam size for beam search\n",                      beam_size);
  fprintf(stderr, "  -wt N,     --word-thold N      [%-7.2f] word timestamp probability threshold\n",         word_thold);
  fprintf(stderr, "  -et N,     --entropy-thold N   [%-7.2f] entropy threshold for decoder fail\n",           entropy_thold);
  fprintf(stderr, "  -lpt N,    --logprob-thold N   [%-7.2f] log probability threshold for decoder fail\n",   logprob_thold);
  fprintf(stderr, "  -su,       --speed-up          [%-7s] speed up audio by x2 (reduced accuracy)\n",        speed_up ? "true" : "false");
  fprintf(stderr, "  -tr,       --translate         [%-7s] translate from source language to english\n",      translate ? "true" : "false");
  fprintf(stderr, "  -di,       --diarize           [%-7s] stereo audio diarization\n",                       diarize ? "true" : "false");
  fprintf(stderr, "  -nf,       --no-fallback       [%-7s] do not use temperature fallback while decoding\n", no_fallback ? "true" : "false");
  fprintf(stderr, "  -osrt,     --output-srt        [%-7s] output result in a srt file\n",                    output_srt ? "true" : "false");
  fprintf(stderr, "  -fp,       --font-path         [%-7s] path to a monospace font for karaoke video\n",     font_path.c_str());
  fprintf(stderr, "  -of FNAME, --output-file FNAME [%-7s] output file path (without file extension)\n",      "");
  fprintf(stderr, "  -ps,       --print-special     [%-7s] print special tokens\n",                           print_special ? "true" : "false");
  fprintf(stderr, "  -pc,       --print-colors      [%-7s] print colors\n",                                   print_colors ? "true" : "false");
  fprintf(stderr, "  -pp,       --print-progress    [%-7s] print progress\n",                                 print_progress ? "true" : "false");
  fprintf(stderr, "  -nt,       --no-timestamps     [%-7s] do not print timestamps\n",                        no_timestamps ? "true" : "false");
  fprintf(stderr, "  -l LANG,   --language LANG     [%-7s] spoken language ('auto' for auto-detect)\n",       language.c_str());
  fprintf(stderr, "  -dl,       --detect-language   [%-7s] exit after automatically detecting language\n",    detect_language ? "true" : "false");
  fprintf(stderr, "             --prompt PROMPT     [%-7s] initial prompt\n",                                 prompt.c_str());
  fprintf(stderr, "  -m FNAME,  --model FNAME       [%-7s] model path\n",                                     model.c_str());
  fprintf(stderr, "  -f FNAME,  --file FNAME        [%-7s] input WAV file path\n",                            "");
  fprintf(stderr, "\n");
  // clang-format on
}

bool WhisperCommandLineParams::Parse() {
  spdlog::info("Parsing command line arguments");
  // clang-format off
  int argc = argvs.size();
  for (int i = 1; i < argc; i++) {
    std::string arg = argvs[i];

    if (arg == "-"){
        fname_inp.push_back(arg);
        continue;
    }

    if (arg[0] != '-') {
        fname_inp.push_back(arg);
        continue;
    }

    if (arg == "-h" || arg == "--help") {
        PrintUsage();
        exit(0);
    }
    else if (arg == "-t"    || arg == "--threads")        { n_threads      = std::stoi(argvs[++i]); }
    else if (arg == "-p"    || arg == "--processors")     { n_processors   = std::stoi(argvs[++i]); }
    else if (arg == "-ot"   || arg == "--offset-t")       { offset_t_ms    = std::stoi(argvs[++i]); }
    else if (arg == "-on"   || arg == "--offset-n")       { offset_n       = std::stoi(argvs[++i]); }
    else if (arg == "-d"    || arg == "--duration")       { duration_ms    = std::stoi(argvs[++i]); }
    else if (arg == "-mc"   || arg == "--max-context")    { max_context    = std::stoi(argvs[++i]); }
    else if (arg == "-ml"   || arg == "--max-len")        { max_len        = std::stoi(argvs[++i]); }
    else if (arg == "-bo"   || arg == "--best-of")        { best_of        = std::stoi(argvs[++i]); }
    else if (arg == "-bs"   || arg == "--beam-size")      { beam_size      = std::stoi(argvs[++i]); }
    else if (arg == "-wt"   || arg == "--word-thold")     { word_thold     = std::stof(argvs[++i]); }
    else if (arg == "-et"   || arg == "--entropy-thold")  { entropy_thold  = std::stof(argvs[++i]); }
    else if (arg == "-lpt"  || arg == "--logprob-thold")  { logprob_thold  = std::stof(argvs[++i]); }
    else if (arg == "-su"   || arg == "--speed-up")       { speed_up       = true; }
    else if (arg == "-tr"   || arg == "--translate")      { translate      = true; }
    else if (arg == "-di"   || arg == "--diarize")        { diarize        = true; }
    else if (arg == "-sow"  || arg == "--split-on-word")  { split_on_word  = true; }
    else if (arg == "-nf"   || arg == "--no-fallback")    { no_fallback    = true; }
    else if (arg == "-osrt" || arg == "--output-srt")     { output_srt     = true; }
    else if (arg == "-fp"   || arg == "--font-path")      { font_path      = argvs[++i]; }
    else if (arg == "-of"   || arg == "--output-file")    { fname_out.emplace_back(argvs[++i]); }
    else if (arg == "-ps"   || arg == "--print-special")  { print_special  = true; }
    else if (arg == "-pc"   || arg == "--print-colors")   { print_colors   = true; }
    else if (arg == "-pp"   || arg == "--print-progress") { print_progress = true; }
    else if (arg == "-nt"   || arg == "--no-timestamps")  { no_timestamps  = true; }
    else if (arg == "-l"    || arg == "--language")       { language       = argvs[++i]; }
    else if (arg == "-dl"   || arg == "--detect-language"){ detect_language= true; }
    else if (                  arg == "--prompt")         { prompt         = argvs[++i]; }
    else if (arg == "-m"    || arg == "--model")          { model          = argvs[++i]; }
    else if (arg == "-f"    || arg == "--file")           { fname_inp.emplace_back(argvs[++i]); }
    else {
        spdlog::error("Unknown argument: {}", arg);
        return false;
    }
  }

  return true;
  // clang-format on
}

}  // namespace whisper
