#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <string_view>

namespace whisper::utlis {

template <void (*log_fun)(std::string_view) = nullptr>
class ScopeTimer {
 public:
  inline ScopeTimer(const char* name)
      : name_(name), start_(std::chrono::system_clock::now()) {
  }

  void Stop() {
    if (name_ == nullptr) {
      return;
    }
    auto cost_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - start_);
    std::stringstream ss;
    ss << "[⌚ScopeTimer⌚] Scope '" << name_ << "', time cost: " << cost_ms.count()
       << " ms";
    if constexpr (log_fun != nullptr) {
      log_fun(ss.str());
    } else {
      std::cout << ss.str() << std::endl;
    }
    name_ = nullptr;
  }

  void Reset(const char* name) {
    Stop();
    name_  = name;
    start_ = std::chrono::system_clock::now();
  }

  inline ~ScopeTimer() {
    Stop();
  }

 private:
  const char* name_;
  std::chrono::system_clock::time_point start_;
};

}  // namespace whisper