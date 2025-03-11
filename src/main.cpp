#include "Output.h"
#include "RestAPI.h"
#include "Traffic.h"
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define JSON_DLL
#endif

int main(int argc, char* argv[]) {
  // Get all Traffic events
  // TODO: Asynchronously call this in a loop in its own thread
  // We want this to be spun off into a bg thread while the REST server runs in main
  
  Traffic::fetchEvents();
  // Get cameras
  Traffic::fetchCameras();
  Traffic::printEvents();
  
  while(true) {
    auto time = Output::currentTime();
    std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n' << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
    Traffic::fetchEvents();
  }

  // Create and start the API server
  RestAPI::ServerApp restServer;
  return restServer.run(argc, argv);

  return 0;
}
