#include <iostream>

#include "whisper.h"
#include "whisper_echo/whisper_context.h"
#include "whisper_echo/whisper_params.h"

using whisper::WhisperContext;
using whisper::WhisperParams;

int main(int argc, char** argv) {
  WhisperParams params(argc, argv);

  auto context = WhisperContext::FromFile(params.model);

  return 0;
}
