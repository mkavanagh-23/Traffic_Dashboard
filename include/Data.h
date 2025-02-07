#ifndef DATA_H
#define DATA_H

#include <string>
#include <json/json.h>

// This file holds all functionality for retrieving and filtering basic data from CURL in XML and JSON formats

namespace cURL {
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::string getData(const std::string& url);
} // namespace cURL

namespace JSON {
Json::Value parseData(const std::string& jsonData);
} // namespace JSON

#endif
