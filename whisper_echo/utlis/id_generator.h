#include <atomic>

namespace whisper {

class TimestampIDGenerator {
 public:
  static int64_t GenerateId();

 private:
  std::atomic<int64_t> counter;

  // Private constructors to prevent direct construction
  TimestampIDGenerator();
  TimestampIDGenerator(const TimestampIDGenerator&) = delete;  // Prevent copy constructor
  void operator=(const TimestampIDGenerator&)       = delete;

  static TimestampIDGenerator& Instance();
  int64_t GenerateIdImpl();
};

}  // namespace whisper
