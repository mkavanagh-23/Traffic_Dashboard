#include "Data.h"
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

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <video source stream/url>" << std::endl;
    return 1;
  }

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromLocation);
  video.play();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  video.stop();

  // Test environment variable parsing from .env
  dotenv::init();
  std::string NYSDOT_API = std::getenv("NYSDOT_API_KEY");

  // Test cURL parsing
  std::string url = "https://511ny.org/api/getevents/?format=json&key=";
  url += NYSDOT_API;
  std::string responseStr = cURL::getData(url);
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << responseStr;

  return 0;
}
