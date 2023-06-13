#pragma once

#include <string>

namespace whisper {

class WhisperApp {
 public:
  struct Config {
    std::string host;
    int port;

    static Config Default();
  };

  class Builder;
  friend class Builder;

  ~WhisperApp() = default;

  void Run();

 private:
  Config config_;

  explicit WhisperApp(Config&& config);
  explicit WhisperApp(const Config& config);
};

class WhisperApp::Builder {
 public:
  Builder();
  ~Builder();

  Builder& SetHost(const std::string& host);
  Builder& SetPort(int port);

  WhisperApp Build();

 private:
  Config config_;
};

}  // namespace whisper