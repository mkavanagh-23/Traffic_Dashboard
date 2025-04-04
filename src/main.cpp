#include "RestAPI.h"
#include "Traffic.h"
#include "DataUtils.h"
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

#ifdef _WIN32
#define JSON_DLL
#endif

void getTrafficData() {
  while(true) {
    Traffic::fetchEvents();
    Output::logger.flush();
    auto time = Time::currentTime_t();
    std::cout << "\nLast updated: " << std::put_time(localtime(&time), "%T") << '\n';
    std::cout << "Found " << Traffic::mapEvents.size() << " matching traffic events.\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(60));
  }
}

// Cleanup function to join the thread
void cleanupThread(std::thread& t) {
    if (t.joinable()) 
        t.join();
}

int main(int argc, char* argv[]) {
  // TODO:
  // Listen for exit command and clean-up before terminating

  // Spin up the data selection backend
  std::thread dataThread(getTrafficData);

  // Create and start the API server
  RestAPI::ServerApp restServer;
  restServer.run(argc, argv);

  // Clean up the data thread
  cleanupThread(dataThread);

  return 0;
}
