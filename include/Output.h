#ifndef OUTPUT_H
#define OUTPUT_H

#include <ctime>
#include <string>

namespace Output {
namespace Colors {
  constexpr std::string RED = "\033[31m";
  constexpr std::string GREEN = "\033[32m";
  constexpr std::string YELLOW = "\033[33m";
  constexpr std::string BLUE = "\033[34m";
  constexpr std::string MAGENTA = "\033[35m";
  constexpr std::string CYAN = "\033[36m";
  constexpr std::string END = "\033[0m";
}

std::time_t currentTime();
}

#endif
