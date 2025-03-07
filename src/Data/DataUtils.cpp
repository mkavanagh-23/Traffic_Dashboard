#include "DataUtils.h"
#include "Traffic/NYSDOT.h"
#include "Traffic/MCNY.h"
#include "Traffic/ONMT.h"
#include "Output.h"
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <rapidxml.hpp>

// TODO:
// Refactor XML parsing to create and return an XML document object rather than take it in by reference
// This will allow us to better maintain DRY principles

namespace cURL {

// Write the header data
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
  size_t totalSize{ size * nitems };
  std::vector<std::string>* headers = static_cast<std::vector<std::string>*>(userdata);

  std::string header(buffer, totalSize);
  headers->push_back(header);

  return totalSize;
}

// Write the data to the string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize{ size * nmemb };    // Calculate the size of the callback buffer
  output->append((char*)contents, totalSize); // Store contents of callback buffer in output
  return totalSize;
}

// Fetch a data string from a remote source
std::tuple<Result, std::string, std::vector<std::string>> getData(const std::string& url){
  CURL* curl{ curl_easy_init() };   // cURL malloc
  std::string responseData;         // Create a string to hold the data
  std::vector<std::string> headers; // Create a vector to hold the response headers
  
  // Check for successful initialization
  if(!curl) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to initialize cURL." << Output::Colors::END << '\n';
    return { Result::INIT_FAILED, "", {} };
  }
  
  // Set cURL options
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);     // Set the data to write
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
  // Set up header handling
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback); // Custom function to capture headers
  curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);           // Pass headers vector to function
    
  // Retrieve the data
  CURLcode res = curl_easy_perform(curl);

  // Check for errors
  if(res != CURLE_OK) {
    std::cerr << Output::Colors::RED << "[cURL] Error retrieving data: " << curl_easy_strerror(res) << ".\n";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "", {} };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "", {} };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "", {} };
    else
      return { Result::REQUEST_FAILED, "", {} };
  }

  // Cleanup the cURL memory
  curl_easy_cleanup(curl);
  return { Result::SUCCESS, responseData, headers };
}

// Extract the content-type header from the response
std::string getContentType(const std::vector<std::string>& headers) {
  // Iterate through each header
  for(const auto& header : headers) {
    // Check for the "Content-Type" header
    if(header.find("Content-Type:") != std::string::npos) {
      // Set an iterator to the start of the value string
      size_t pos = header.find(":");
      // Check if the key has a value
      if(pos != std::string::npos) {
        return header.substr(pos + 1);  // Ectract the value that follows the ":"
      }
    }
  }
  return "";  // Return an empty string if the header wasn't found
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
  if(!MCNY::getEvents())
    return false;
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
