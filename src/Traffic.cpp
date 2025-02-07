#include "Traffic.h"

#include <string>
#include <json/json.h>
#include <iostream>
#include <ostream>
#include <sstream>

namespace Traffic {

/********************* NYSDOT Traffic Data (511ny.org) ************************/

namespace NYSDOT {

std::string API_KEY;
EventMap<Event> events; // Key = "ID"

// Parse events from a Json data stream onto the global event map
bool parseEvents(const std::string& jsonData) {
  // Track any parsing failures
  bool success{ true };

  // Set up Json parsing objects
  Json::CharReaderBuilder builder;
  Json::Value root;                 // Root node of the parsed objects
  std::istringstream data(jsonData);
  std::string errs;                 // Hold errors in a string

  // Parse the string into the root Value object
  if(!Json::parseFromStream(builder, data, &root, &errs)) {
    // If initial parsing fails, send an error message and return without processing
    std::cerr << "[NYSDOT] Error parsing JSON (is it a valid stream?): " << errs << std::endl;
    return false;
  }

  // TODO:  
  //    Process each event from the root object
  //    Check each Event ID against the map to see if it already exists to determine update or new
  //    Process the event and continue or create an event store on the map
  
  // Return if any failures occurred
  return success;
}

// Overload operator<< to print an event object
std::ostream &operator<<(std::ostream &out, const Event &event) {
  out << "\nEvent ID: " << event.ID << "  |  " << event.EventType << " (" << event.EventSubType << ")"
      << "\nRegion: " << event.RegionName << "  |  " << event.CountyName << " County"
      << "\nSeverity: " << event.Severity
      << '\n' << event.RoadwayName << ' ' << event.DirectionOfTravel
      << '\n' << event.PrimaryLocation << ' ' << event.SecondaryLocation
      << "\nLanes: " << event.LanesAffected << ' ' << event.LanesStatus
      << "\n Last Updated: " << event.LastUpdated
      << std::endl;
  return out;
}
} // namespace NYSDOT





/************************ Monroe County Dispatch Feed *************************/

namespace MCNY {

} // namespace MCNY
} // namespace Traffic
