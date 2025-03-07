#ifndef DATA_H
#define DATA_H

#include <string>
#include <json/json.h>
#include <rapidxml.hpp>
#include <unordered_map>

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

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::pair<Result, std::string> getData(const std::string& url);
} // namespace cURL

namespace JSON {
Json::Value parseData(const std::string& jsonData);
} // namespace JSON

namespace XML {
void parseData(rapidxml::xml_document<>& document, std::string xmlData);
} // namespace XML


// Define shared data for various traffic event sources
namespace Traffic {
// Create a template for EventMaps of different event types
template<typename T1, typename T2>
using TrafficMap = std::unordered_map<T1, T2>;

struct BoundingBox {
  const double longLeft;
  const double longRight;
  const double latTop;
  const double latBottom;

  bool contains(const std::pair<double, double>& coordinate) const;
};

// Get all traffic events
bool getEvents();
bool getCameras();
}

struct Location {
  double latitude;
  double longitude;
};

#endif
