#include "main.h"
#include "Output.h"
#include "RestAPI.h"
#include "Traffic.h"
#include "DataUtils.h"
#include <atomic>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

// Atomic flag for program end
std::atomic<bool> programEnd(false);

// Wait for user input to continue
void waitForEnter() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cout << "\nPress Enter to continue...";
  std::cin.get();
}

// Get all traffic data
void getTrafficData() {
  while(!programEnd) {
    Traffic::fetchEvents();
    Output::logger.flush();
    // Sleep in intervals
    int interval_seconds = 5;
    int sleep_seconds = 60;
    int sleep_intervals = sleep_seconds / interval_seconds;
    for(int i = 0; i < sleep_intervals && !programEnd; i++)
      std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
  }
}

// Cleanup function to join the thread
void cleanupThread(std::thread& t) {
    if (t.joinable()) 
      t.join();
}

// Display main menu
void displayMenu() {
  std::cout << "\n***** Traffic Events *****\n"
            << "1. Print all events\n"
            << "2. Print events for region\n"
            << "3. End program\n";

  std::cout << "\n\nEnter a choice: ";
}

// Display regional menu
void regionMenu() {
  std::cout << "\n***** Traffic Events (Regional) *****\n"
            << "1. Syracuse\n"
            << "2. Rochester\n"
            << "3. Buffalo\n"
            << "4. Albany\n"
            << "5. Binghamton\n"
            << "6. Toronto\n"
            << "7. Montreal\n"
            << "8. Ottawa\n"
            << "\n9. BACK\n";
  
  std::cout << "\n\nEnter a choice: ";
}

// Prompt user for region choice
void regionChoice() {
  int choice{0};
  while(choice != 9) {
    Output::clearConsole();
    regionMenu();
    std::cin >> choice;

    // Process choice
    switch(choice) {
      // Handle regional cases
      case 1:
        Traffic::printEvents(Traffic::Region::Syracuse);
        waitForEnter();
        break;
      case 2:
        Traffic::printEvents(Traffic::Region::Rochester);
        waitForEnter();
        break;
      case 3:
        Traffic::printEvents(Traffic::Region::Buffalo);
        waitForEnter();
        break;
      case 4:
        Traffic::printEvents(Traffic::Region::Albany);
        waitForEnter();
        break;
      case 5:
        Traffic::printEvents(Traffic::Region::Binghamton);
        waitForEnter();
        break;
      case 6:
        Traffic::printEvents(Traffic::Region::Toronto);
        waitForEnter();
        break;
      case 7:
        Traffic::printEvents(Traffic::Region::Montreal);
        waitForEnter();
        break;
      case 8:
        Traffic::printEvents(Traffic::Region::Ottawa);
        waitForEnter();
        break;
      case 9:
        // Return to program
        return;
      default:
        std::cout << "Invalid choice! Try again.";
        waitForEnter();
        break;
    }
  }
}

// Run the main debug console
void debugConsole() {
  int choice{0};
  while(choice != 3) {
    // Clear the screen
    Output::clearConsole();
    displayMenu();
    std::cin >> choice;
    
    // Process choice
    switch(choice) {
      case 1:
        // Print all events
        Traffic::printEvents();
        waitForEnter();
        break;
      case 2:
        // Print regional events
        regionChoice();
        break;
      case 3:
        // Terminate program
        std::cout << "Terminating program!\n";
        programEnd = true;
        return;
      default:
        std::cout << "Invalid choice! Try again.";
        waitForEnter();
        break;
    }
  }
}

int main() {

  // Spin up the data processing thread
  std::thread dataThread(getTrafficData);
  // Spin up the api thread
  std::thread apiThread(RestAPI::startApiServer);

  // Wait a bit for services to start
  std::cout << "Staring services...\n";
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // Main application logic here
  // Start the debug console for runtime testing
  debugConsole();

  // Clean up the data thread
  cleanupThread(apiThread);
  cleanupThread(dataThread);

  return 0;
}
