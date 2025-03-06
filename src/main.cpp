#include "Traffic/NYSDOT.h"
#include "DataUtils.h"
#include "Output.h"
#include "Data/RestAPI.h"
#include <ctime>
#include <csignal>
#include <cstdlib>
#include <iomanip>
#include <iostream>

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

int main(int argc, char* argv[]) {

  // Source API key from ENV
  // TODO: Will be placed in global Setup() function
  if(!Traffic::NYSDOT::getEnv())
    return 1;

  // Get cameras
  if(!Traffic::getCameras())
    return 1;


  // Get all Traffic events
  // TODO: Asynchronously call this in a loop in its own thread
  // We want this to be spun off into a bg thread while the REST server runs in main
  if(!Traffic::getEvents())
    return 1;
  auto time = Output::currentTime();
  std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n' << std::endl;

  // Create and start the API server
  RestAPI::ServerApp restServer;
  return restServer.run(argc, argv);

  return 0;
}
