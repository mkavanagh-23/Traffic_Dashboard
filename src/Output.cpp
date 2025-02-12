#include "Output.h"

#include <chrono>
#include <ctime>

namespace Output {
std::time_t currentTime() {
  auto tempTime = std::chrono::system_clock::now();
  auto time = std::chrono::system_clock::to_time_t(tempTime);
  return time;
}
}
