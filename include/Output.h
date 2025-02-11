#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>

namespace Output {
namespace Colors {
  constexpr std::string RED = "\033[31m";
  constexpr std::string YELLOW = "\033[33m";
  constexpr std::string GREEN = "\033[32m";
  constexpr std::string BLUE = "\033[34m";
  constexpr std::string END = "\033[0m";
}
}

#endif
