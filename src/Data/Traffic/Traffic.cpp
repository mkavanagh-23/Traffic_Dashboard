#include "Traffic.h"
#include "DataUtils.h"
#include "Output.h"
#include <string>
#include <iostream>

namespace Traffic {

std::string NYSDOT_API_KEY;
bool NYSDOT_API_KEY_sourced{ false };

void getEnv() {
  // Retrieve API Key from local environment
  const char* API_KEY = std::getenv("NYSDOT_API_KEY");

  // Check for valid sourcing
  if(API_KEY) {
    NYSDOT_API_KEY = API_KEY;
    std::cout << Output::Colors::GREEN << "[ENV] Successfully sourced API key from local environment.\n" << Output::Colors::END;
  }

}

void getEvents(std::string url) {
  // Check for NYSDOT (do we need to add our API key?)
  if(url.find("511ny.org") != std::string::npos) {
    url += NYSDOT_API_KEY;
  }

  // Retrieve data with cURL
  auto [result, data, headers] = cURL::getData(url);

  // Check for successful extraction
  if(result == cURL::Result::SUCCESS) {
    // Extract the "Content-Type" header
    std::string contentType = cURL::getContentType(headers);

    // Check for valid JSON or XML response
    if(contentType.find("application/json") != std::string::npos) {
      // Process data as JSON
    } else if(contentType.find("application/xml") != std::string::npos) {
      // Process data as XML
    } else {
      std::cerr << Output::Colors::RED << "[cURL] ERROR: Unsupported \"Content-Type\": '" <<  contentType << '\n' << Output::Colors::END;
    }
  } else {
    // Handle the error case...
  }
}

} // namespace Traffic
