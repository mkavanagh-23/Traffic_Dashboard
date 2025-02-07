#include "Data.h"
#include "Traffic.h"
#include "Video.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <cstdlib>
#include <dotenv.h>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define JSON_DLL
#endif

/* TODO: 
 *  Parse and store information from NYSDOT REST API
 *  Parse and store information for MCNY RSS feed
 *  GUI Window with a top bar that allows for picking between markets
 *  Left sidebar contains a selectable list of currently active incidents
 *  Incidents are refreshed every one minute
 *  Main content pane contains a "Details" view that contains expanded details
 *  include an embedded video stream (if available) and an embedded map 
 *  Do we want to display embedded video feed, or just keyframes?
 *  Incorporate JSON and XML/RSS parsing
 *  Source videos via API - libcurl successfully installed
 *  Mapping coordinates to regions and markets
 *  Severity coding
 *  Embed Goole Maps objects
 *  Create a dynamic graphical interface
 */

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << "\033[31musage: " << argv[0] << " <video source stream/url>\033[0m" << std::endl;
    return 1;
  }

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromLocation);
  //video.play();
  //std::this_thread::sleep_for(std::chrono::seconds(2));
  //video.stop();

  // Test environment variable parsing from .env
  dotenv::init();
  Traffic::NYSDOT::API_KEY = std::getenv("NYSDOT_API_KEY");
  if(Traffic::NYSDOT::API_KEY.empty()) {
    std::cerr << "\033[31m[dotEnv] Failed to retrieve 'NYSDOT_API_KEY'.\nBe sure you have defined it in '.env'.\n"
              << "Exiting program.\033[0m\n";
    return 1;
  }
  std::cout << "\033[32m[dotEnv] Successfully sourced API key from local environment.\033[0m\n";

  // Test cURL parsing
  std::string url{ "https://511ny.org/api/getevents/?format=json&key=" + Traffic::NYSDOT::API_KEY };
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << "\033[31mExiting program.\033[0m\n";
    return 1;
  }
  std::cout << "\033[32m[cURL] Successfully retrieved JSON from 511ny.\033[0m\n";

  // Test JSON Parsing
  JSON::parseData(responseStr);

  return 0;
}
