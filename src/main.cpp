#include "Output.h"
#include "Data/RestAPI.h"
#include "Traffic.h"
#include <ctime>
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


  // Get cameras


  // Get all Traffic events
  // TODO: Asynchronously call this in a loop in its own thread
  // We want this to be spun off into a bg thread while the REST server runs in main
  Traffic::fetchEvents();
  auto time = Output::currentTime();
  std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n' << std::endl;

  // Create and start the API server
  RestAPI::ServerApp restServer;
  return restServer.run(argc, argv);

  return 0;
}
