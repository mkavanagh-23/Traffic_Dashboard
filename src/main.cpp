#include "RestAPI.h"
#include "Traffic.h"
#include "DataUtils.h"
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
  // We want to run an update thread, a worker thread for queued processing, a listen thread for the REST server
  // Going to need to design a soultion with mutexes to avoid UB
  
  // Get cameras
  //Traffic::fetchCameras();
  
  while(true) {
    Traffic::fetchEvents();
    Traffic::printEvents();
    auto time = Time::currentTime_t();
    std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n';
    std::cout << "Found " << Traffic::mapEvents.size() << " matching traffic events.\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }

  // Create and start the API server
  RestAPI::ServerApp restServer;
  return restServer.run(argc, argv);

  return 0;
}
