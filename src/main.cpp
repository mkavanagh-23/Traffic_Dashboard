#include "NYSDOT.h"
#include "DataUtils.h"
#include "Output.h"
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

int main()
{
  // Register signal handler
  signal(SIGINT, signalHandler);

  // Source API key from ENV
  // TODO: Will be placed in global Setup() function
  if(!Traffic::NYSDOT::getEnv())
    return 1;

  // Get cameras
  if(!Traffic::getCameras())
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
