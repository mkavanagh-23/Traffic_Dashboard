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
    if(!parsedEvent.isObject()) {
      std::cerr << "\033[31m[NYSDOT] Failed parsing event (is the JSON valid?)\033[0m\n";
      return false;
    }
    /* TODO:  
     *    Check each Event ID against the map to see if it already exists to determine update or new
     *    Process the event and continue or create an event store on the map
     */
  }
  return true;
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
