#include "Data.h"
#include <string>
#include <iostream>
#include <curl/curl.h>

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
}
