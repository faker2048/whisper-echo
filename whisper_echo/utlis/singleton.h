#pragma once

#include <memory>

namespace whisper {
/*
 * Usage example:
 * // Using a type alias to make the code more readable
 * using MyInstanceSingleton = whisper::Singleton<MyInstance>;
 *
 * // Initialize the singleton with an instance of MyInstance
 * auto myInstance = std::make_unique<MyInstance>();
 * MyInstanceSingleton::GetSingletonInstance().Init(std::move(myInstance));
 *
 * // Or initialize the singleton with constructor arguments
 * MyInstanceSingleton::GetSingletonInstance().Init(42, "example");
 *
 * // Get the instance
 * MyInstance* instance = MyInstanceSingleton::GetSingletonInstance().Instance();
 */
template <typename InstanceType>
class Singleton {
 public:
  static Singleton& GetSingletonInstance() {
    static Singleton instance;
    return instance;
  }

  // Unique ptr initialization
  void Init(std::unique_ptr<InstanceType> instance) {
    if (instance == nullptr) {
      throw std::runtime_error("Singleton instance is nullptr");
    }
    if (instance_ != nullptr) {
      throw std::runtime_error("Singleton instance already initialized");
    }
    instance_ = std::move(instance);
  }

  // Parameter pack initialization
  template <typename... Args>
  void Init(Args&&... args) {
    if (instance_ != nullptr) {
      throw std::runtime_error("Singleton instance already initialized");
    }
    instance_ = std::make_unique<InstanceType>(std::forward<Args>(args)...);
  }

  InstanceType* Instance() {
    return instance_.get();
  }

  const InstanceType* Instance() const {
    return instance_.get();
  }

 private:
  Singleton()                            = default;
  Singleton(const Singleton&)            = delete;
  Singleton& operator=(const Singleton&) = delete;

  std::unique_ptr<InstanceType> instance_ = nullptr;
};

}  // namespace whisper