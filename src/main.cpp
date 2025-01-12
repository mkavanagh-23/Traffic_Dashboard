#include "Video.h"
#include "json/json.h"
#include <SDL2/SDL.h>
#include <curl/curl.h>
#include <thread>
#include <iostream>
#include <chrono>
#include <string>
#include <memory>


#ifdef _WIN32
    #define JSON_DLL
#endif
// Parse and store information from NYSDOT REST API
// Parse and store information for MCNY RSS feed
// GUI Window with a top bar that allows for picking between markets
// Left sidebar contains a selectable list of currently active incidents
// Incidents are refreshed every one minute
// Main content pane contains a "Details" view that contains expanded details included an embedded video stream (if available) and an embedded map
// Do we want to display embedded video feed, or just keyframes?
//
// Incorporate JSON and XML/RSS parsing
// Learn how to incorporate API key safely into project (perhaps sourced from ENV)?
// Source videos via API - libcurl successfully installed
// Mapping coordinates to regions and markets
// Severity coding
// Embed Goole Maps objects
// Create a dynamic graphical interface

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <video source stream/url>" << std::endl;
    return 1;
  }

  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "Failed to initialize SDL" << std::endl;
    return 1;
  }
  else {
    std::cout << "SDL Initialized Successfully!\n";
  }

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromPath);

  /******* DEBUG TESTING FOR LIBCURL *******/

  CURL *curl;
  CURLcode res;

  std::cout << "Begin libcurl debug test:\n";

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    /* Perform the request, res gets the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  /********* DEBUG TESTING FOR LIBJSONCPP ***********/
  std::cout << "Begin JSON parsing test\n";
  const std::string rawJson = R"({"Age": 20, "Name": "colin"})";
  const auto rawJsonLength = static_cast<int>(rawJson.length());
  constexpr bool shouldUseOldWay = false;
  JSONCPP_STRING err;
  Json::Value root;
  
  if (shouldUseOldWay) {
    Json::Reader reader;
    reader.parse(rawJson, root);
  } else {
    Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
                       &err)) {
      std::cout << "error: " << err << std::endl;
      return EXIT_FAILURE;
    }
  }
  const std::string name = root["Name"].asString();
  const int age = root["Age"].asInt();
  
  std::cout << name << std::endl;
  std::cout << age << std::endl;

  /****** DEBUG VIDEO PLAYBACK *******/

  // Play the video
  video.play();
  std::cout << "Now playing video: " << video.getSource() << '\n';
  std::cout << "Type: " << video.getTypeStr() << '\n';

  // Play for 15 seconds to test proper playback
  std::this_thread::sleep_for(std::chrono::seconds(15));

  // Stop media playback
  video.stop();

  SDL_Quit();

  return 0;
}
