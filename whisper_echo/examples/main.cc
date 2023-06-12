#include <iostream>

#include "whisper.h"
#include "whisper_echo/whisper_params.h"

int main(int argc, char** argv) {
  whisper::WhisperParams params(argc, argv);
  params.PrintUsage();
  return 0;
}
