#include "Traffic.h"

#include <string>
#include <iostream>

namespace Traffic {

/********************* NYSDOT Traffic Data (511ny.org) ************************/

namespace NYSDOT {

std::string API_KEY;
EventMap<Event> eventMap; // Key = "ID"


bool parseEvents(const Json::Value& events){
  // Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check if event is a valid object
    if(!parsedEvent.isObject()) {
      std::cerr << "\033[31m[NYSDOT] Failed parsing event (is the JSON valid?)\033[0m\n";
      return false;
    }
    // Store the event on the map
    if(processEvent(parsedEvent)) {
      std::cout << "[NYSDOT] Stored event " << parsedEvent["ID"].asString() << '\n'
                << parsedEvent["RegionName"].asString() << "  |  " << parsedEvent["RoadwayName"].asString() << '\n';
    }
  }
  return true;
}

/* TODO:  
 *    Check each Event ID against the map to see if it already exists to determine update or new
 *    Process the event and continue or create an event store on the map
 */

bool processEvent(const Json::Value& parsedEvent) {
  // Extract the event ID
  std::string eventID{ parsedEvent["ID"].asString() };
  // Extract the region
  std::string eventRegion{ parsedEvent["RegionName"].asString() };

  // Check against matching region(s)
  if(eventRegion == "Central Syracuse Utica Area") {
    return true;
  }
  return false;
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
