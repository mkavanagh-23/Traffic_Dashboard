#include "ONMT.h"
#include "Traffic/NYSDOT.h"
#include "Traffic/MCNY.h"
#include "Video.h"
#include "Output.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <ctime>
#include <csignal>
#include <atomic>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define JSON_DLL
#endif

/* TODO: 
 *  Add methods for accessing cameras from NYSDOT API
 *  GUI Window with a top bar that allows for picking between markets
 *  Left sidebar contains a selectable list of currently active incidents
 *  Incidents are refreshed every one minute
 *  Main content pane contains a "Details" view that contains expanded details
 *  include an embedded video stream (if available) and an embedded map 
 *  Do we want to display embedded video feed, or just keyframes?
 *  Severity coding
 *  Embed Goole Maps objects
 *  Create a dynamic graphical interface
 */

std::atomic<bool> running(true);

void signalHandler(int signum) {
  std::cout << "\nInterrupt signal received (" << signum << "). Terminating program...\n";
  running = false;
}

int main(int argc, char** argv)
{
  // Check for valid arguments
  if (argc < 2) {
    std::cerr << Output::Colors::RED << "[ERROR] Usage: " << argv[0] << " <video source stream/url>" << Output::Colors::END << std::endl;
    return 1;
  }

  // Register signal handler
  signal(SIGINT, signalHandler);

  // Create the video object from the stream URL
  Video video(argv[1], VLC::Media::FromLocation);
  video.play();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  video.stop();
  
  // Source API key from ENV
  // TODO: Will be placed in global Setup() function
  if(!Traffic::NYSDOT::getEnv())
    return 1;

  while(running) {
    // Get all Traffic events
    if(!Traffic::getEvents())
      return 1;
    auto time = Output::currentTime();
    std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n' << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }

  return 0;
}
