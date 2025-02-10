#include "Data.h"
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <rapidxml.hpp>

namespace cURL {
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize{ size * nmemb };    // Calculate the size of the callback buffer
  output->append((char*)contents, totalSize); // Store contents of callback buffer in output
  return totalSize;
}

std::string getData(const std::string& url) {
  CURL* curl{ curl_easy_init() };  // cURL malloc
  std::string responseData;   // Create a string to hold the data
  
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the cURL url
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);     // Set the data to write
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
    
    // Retrieve the data
    CURLcode res = curl_easy_perform(curl);

    // Check for errors
    if(res != CURLE_OK) {
      std::cerr << "\033[31m[cURL] Error retrieving data: " << curl_easy_strerror(res) << ".\033[0m\n";
    }
    
    // Cleanup the cURL object
    curl_easy_cleanup(curl);
  } 
  else {
    std::cerr << "\033[31m[cURL] Failed to initialize cURL.\033[0m\n";
  }
  
  return responseData;
}
} // namespace cURL

namespace JSON {
// Parse events from a Json data stream
Json::Value parseData(const std::string& jsonData) {
  // Set up Json parsing objects
  Json::CharReaderBuilder builder;
  Json::Value root;                 // Root node of the parsed objects
  std::istringstream data(jsonData);
  std::string errs;                 // Hold errors in a string

  // Parse the string into the root Value object
  if(!Json::parseFromStream(builder, data, &root, &errs)) {
    // If initial parsing fails, send an error message
    std::cerr << "\033[31m[JSON] Error parsing JSON (is it a valid stream?): " << errs << ".\033[0m\n";
    // TODO: Throw an exception if we do not parse from stream
    // Should also throw an exception in the underlying/preceding curl function
  }
  std::cout << "\033[32m[JSON] Successfully parsed events from JSON stream.\033[0m\n";
  return root; // Return the parsed root of objects
}
} // namespace JSON

namespace XML {
// Parse events from an XML data stream
void parseData(rapidxml::xml_document<>& document, std::string xmlData) {
  // Parse the XML into a document object
  document.parse<0>(&xmlData[0]);
}

} // namespace XML
