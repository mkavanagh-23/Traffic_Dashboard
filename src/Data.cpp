#include "Data.h"
#include <string>
#include <curl/curl.h>

namespace cURL {
  size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;    // Calculate the size of the callback buffer
    output->append((char*)contents, totalSize); // Store contents of callback buffer in output
    return totalSize;
  }

  std::string getData(const std::string& url) {
    CURL* curl = curl_easy_init();  // cURL malloc
    std::string responseData;   // Create a string to hold the data
    
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Set the cURL url
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);     // Set the data to write
      
      // Retrieve the data
      curl_easy_perform(curl);
      curl_easy_cleanup(curl);
    }
    
    return responseData;
  }
}
