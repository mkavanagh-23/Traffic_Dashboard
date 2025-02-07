#include "Traffic.h"

#include <string>

namespace Traffic {

/********************* NYSDOT Traffic Data (511ny.org) ************************/

namespace NYSDOT {

std::string API_KEY;
EventMap<Event> events; // Key = "ID"

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
