#include "OTT.h"
#include "DataUtils.h"
#include "Output.h"
#include <optional>
#include <iostream>
#include <sstream>
#include <string>

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
}
}
