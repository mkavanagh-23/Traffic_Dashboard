#include "ONMT.h"
#include "DataUtils.h"
#include "Output.h"
#include <string>
#include <optional>
#include <tuple>
#include <regex>

namespace Traffic {
namespace ONMT {
const std::string EVENTS_URL{ "https://511on.ca/api/v2/get/event?format=json&lang=en" };
constexpr BoundingBox regionToronto{ -80.099, -78.509, 44.205, 43.137 };
constexpr BoundingBox regionOttawa{ -76.053, -75.089, 45.759, 45.040 };

// Parse a description into an event title, main street, and cross street
std::optional<std::tuple<std::string, std::string, std::string>> parseDescription(const std::string& description) {
  // Define the matching pattern
  std::regex pattern(R"((.+?)\s+on\s+(.+?)\s+at\s+(.+?),)"); // 1 - Title | 2 - mainStreet | 3 - crossStreet 
  std::smatch matches;

  if(std::regex_search(description, matches, pattern)) {
    // Extract our values
    std::string eventTitle = matches[1];
    std::string eventStreet = matches[2];
    std::string eventCross = matches[3];

    return std::make_tuple(eventTitle, eventStreet, eventCross);
  }

  std::string errMsg = "ONMT description does not match (\"" + description + "\")";
  Output::logger.log(Output::LogLevel::ERROR, "REGEX", errMsg);
  return std::nullopt;

}

}
}
