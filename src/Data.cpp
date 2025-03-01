#include "Data.h"
#include "NYSDOT.h"
#include "MCNY.h"
#include "ONMT.h"
#include "Output.h"
#include <string>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <rapidxml.hpp>

namespace cURL {
// Write the data to the string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize{ size * nmemb };    // Calculate the size of the callback buffer
  output->append((char*)contents, totalSize); // Store contents of callback buffer in output
  return totalSize;
}

// Fetch a data string from a remote source
std::pair<Result, std::string> getData(const std::string& url){
  CURL* curl{ curl_easy_init() };  // cURL malloc
  std::string responseData;   // Create a string to hold the data

  if(!curl) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to initialize cURL." << Output::Colors::END << '\n';
    return { Result::INIT_FAILED, "" };
  }
  
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);     // Set the data to write
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
    
  // Retrieve the data
  CURLcode res = curl_easy_perform(curl);

    // Check for errors
  if(res != CURLE_OK) {
    std::cerr << Output::Colors::RED << "[cURL] Error retrieving data: " << curl_easy_strerror(res) << ".\n";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "" };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "" };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "" };
    else
      return { Result::REQUEST_FAILED, "" };
  }
  // Cleanup the cURL object
  curl_easy_cleanup(curl);
  
  return { Result::SUCCESS, responseData };
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
    std::cerr << Output::Colors::RED << "[JSON] Error parsing JSON (is it a valid stream?): " << errs << '.' << Output::Colors::END << '\n';
    // TODO: Throw an exception if we do not parse from stream
    // Should also throw an exception in the underlying/preceding curl function
  }
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed objects from JSON stream." << Output::Colors::END << '\n';
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

namespace Traffic {
bool BoundingBox::contains(const std::pair<double, double>& coordinate) const {
  auto& [latitude, longitude] = coordinate;
  if((latitude >= latBottom && latitude <= latTop) && (longitude >= longLeft && longitude <= longRight))
    return true;

  return false;
}

// Get all traffic events
bool getEvents(){
  // Test event parsing
  if(!NYSDOT::getEvents())
    return false;
  if(!Ontario::getEvents())
    return false;
  //if(!MCNY::getEvents())
  //  return false;
  return true;
}

bool getCameras(){
  if(!NYSDOT::getCameras())
    return false;
  if(!Ontario::getCameras())
    return false;
  return true;
}
}
