#ifndef DATA_H
#define DATA_H

#include <json/json.h>
#include <rapidxml.hpp>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <tuple>
#include <chrono>

// This file holds all functionality for retrieving and filtering basic data from CURL in XML and JSON formats

namespace cURL {

enum class Result {
  SUCCESS,
  UNSUPPORTED_PROTOCOL,
  INIT_FAILED,
  BAD_URL,
  TIMEOUT,
  REQUEST_FAILED
};

// Callback function for writing the header data
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);
// Callback function for writing the result data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
// Fetch and process data from remote url (Result, Data, Headers)
std::tuple<Result, std::string, std::vector<std::string>> getData(const std::string& url);
// Extract the content type from the response headers
std::string getContentType(const std::vector<std::string>& headers);

} // namespace cURL

namespace JSON {
Json::Value parseData(const std::string& jsonData);
} // namespace JSON

namespace XML {
std::unique_ptr<rapidxml::xml_document<>> parseData(std::string& xmlData);
} // namespace XML


// Define shared data for various traffic event sources
namespace Traffic {

struct Location {
  double latitude;
  double longitude;

  friend std::ostream &operator<<(std::ostream &out, const Location &location);
};

struct BoundingBox {
  const double longLeft;
  const double longRight;
  const double latTop;
  const double latBottom;

  bool contains(const Location& coordinate) const;
};

} // namespace Traffic

namespace Time {
using namespace std::chrono;

// Create a local formatted time string for printing from a time point object
std::tm toLocalPrint(const system_clock::time_point& time);

// Adjust a given timepoint by the offset value
void toUTC(system_clock::time_point& timePoint, const std::string& offset);

namespace UNIX {

// Convert a UNIX time value to a chrono object with an optional offset
system_clock::time_point toChrono(const int unixtime, std::optional<std::string> offset);

} // namespace UNIX

namespace RFC2822 {

// Convert a 3-char month string into a 2-digit int
int stoiMonth3(const std::string& month);
// Attempt to convert a given string to
std::optional<system_clock::time_point> toChrono(const std::string& rfc2822);

} // namespace RFC2822

namespace DDMMYYYYHHMMSS {

system_clock::time_point toChrono(const std::string& timeStr);

} // namespace DDMMYYYYHHMMSS
} // namespace Time

#endif
