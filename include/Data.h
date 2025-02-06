#ifndef DATA_H
#define DATA_H

#include <string>

// This file holds all functionality for retrieving and filtering basic data from CURL in XML and JSON formats

namespace cURL {
  size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
  std::string getData(const std::string& url);
}

#endif
