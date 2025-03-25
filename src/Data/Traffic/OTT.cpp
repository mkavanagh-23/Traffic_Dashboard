#include "OTT.h"
#include "DataUtils.h"
#include "Output.h"
#include <optional>
#include <iostream>
#include <sstream>
#include <string>
#include <regex>

namespace Traffic {
namespace OTT {
extern const std::string EVENTS_URL{ "https://traffic.ottawa.ca/service/events" };
extern constexpr BoundingBox regionOttawa{ -76.54, -74.73, 45.86, 44.86 }; 

// Parse location from the JSON string
std::optional<std::pair<double, double>> parseLocation(const std::string& coordinates) {
  // Remove leading and trailing brackets
  std::string stripped = coordinates.substr(1, coordinates.length() -2);

  // Create a stringstream object to extract values
  std::stringstream ss(stripped);
  double longitude, latitude;
  char delimit;

  // Extract the first double
  if(!(ss >> longitude)) {
    std::cerr << Output::Colors::RED << "Failed parsing first coordinate.\n" << Output::Colors::END;
    return std::nullopt;
  }

  // Pass over delimiter
  if(!(ss >> delimit) || delimit != ',') {
    std::cerr << Output::Colors::RED << "Comma delimiter not found.\n" << Output::Colors::END;
    return std::nullopt;
  }

  // Extract the second double
  if(!(ss >> latitude)) {
    std::cerr << Output::Colors::RED << "Failed parsing second coordinate.\n" << Output::Colors::END;
    return std::nullopt;
  }

  return std::make_pair(latitude, longitude);
}

std::optional<roadwayStr> parseHeadline(const std::string& headline) {
  std::regex pattern(R"((\S+(?:\s\S+)*?)\s*([A-Za-z/]+)?\s*(?:at\s*(.*)))");

  std::smatch matches;
  if(std::regex_match(headline, matches, pattern)) {
    if(matches[2].matched) {
      if(matches[3].matched)
        return std::make_tuple(matches[1], matches[2], matches[3]);
      else
        return std::make_tuple(matches[1], matches[2], std::nullopt);
    } else {
      if(matches[3].matched)
        return std::make_tuple(matches[1], std::nullopt, matches[3]);
      else
        return std::make_tuple(matches[1], std::nullopt, std::nullopt);
    }
  }
  return std::nullopt;
}
}
}
