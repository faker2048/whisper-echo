#include "whisper_echo/utlis/id_generator.h"

#include <chrono>

namespace whisper {

TimestampIDGenerator::TimestampIDGenerator() : counter(0) {
}

TimestampIDGenerator& TimestampIDGenerator::Instance() {
  static TimestampIDGenerator instance;
  return instance;
}

int64_t TimestampIDGenerator::GenerateId() {
  return Instance().GenerateIdImpl();
}

int64_t TimestampIDGenerator::GenerateIdImpl() {
  constexpr int kNsPerMs = 1000000;
  int64_t nanos          = std::chrono::system_clock::now().time_since_epoch().count();
  int64_t ms             = nanos / kNsPerMs;
  int64_t id             = ms * kNsPerMs + (counter.fetch_add(1) % kNsPerMs);

  return id;
}

}  // namespace whisper
