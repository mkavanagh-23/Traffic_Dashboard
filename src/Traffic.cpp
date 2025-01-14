#include "Traffic.h"
#include <charconv>
#include <stdexcept>
#include <iostream>

namespace Traffic {
  Date::Date(std::string_view dateStr) {        // Wrap constructor in a try/catch to make sure we catch any raised exceptions
    if(dateStr.size() != 19) {  // Check fot valid date string size, if not throw an exception
      throw std::invalid_argument("Invalid date format provided");
    }

    try {
      day = svtoi(dateStr.substr(0, 2));
      month = svtoi(dateStr.substr(3, 2));
      year = svtoi(dateStr.substr(6, 4));
      hours = svtoi(dateStr.substr(11, 2));
      minutes = svtoi(dateStr.substr(14, 2));
      seconds = svtoi(dateStr.substr(17, 2));
    } 
    catch(const std::exception& error) {
      std::cerr << "[ERROR]: " << error.what() << '\n';
    }
  }

  int Date::svtoi(std::string_view sv) {
    int num;

    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), num);

    if(result.ec != std::errc()) {
      throw std::out_of_range("Critical error converting string_view to int");
    }

    return num;
  }
}
