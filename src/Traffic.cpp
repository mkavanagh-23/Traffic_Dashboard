#include "Traffic.h"

#include <string>
#include <iostream>

namespace Traffic {

/********************* NYSDOT Traffic Data (511ny.org) ************************/
/* TODO:
 *  - Add checks for each key before assignment
 *  - Clean up events once they are cleared
 *  - Error handling - try/catch
 *  - Get camera data
 *      - cURL request should be implemented here rather than in main
 *      - i.e. getEvents() and getCameras()
 *      - Multithreading and loop structure - updates in the backend
 *  - Logging
 */

namespace NYSDOT {

std::string API_KEY;
EventMap<Event> eventMap; // Key = "ID"

// Parse events from the root events object
bool parseEvents(const Json::Value& events){
  // Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check if event is a valid object
    if(!parsedEvent.isObject()) {
      std::cerr << "\033[31m[NYSDOT] Failed parsing event (is the JSON valid?)\033[0m\n";
      return false;
    }
    // Process the event for storage
    processEvent(parsedEvent);
  }
  return true;
}

// Check the event against filter value and store on the map
bool processEvent(const Json::Value& parsedEvent) {
  // Check against matching region(s)
  if( parsedEvent["RegionName"].asString()  == "Central Syracuse Utica Area") {
    // Construct an event object on the map
    eventMap.insert_or_assign(parsedEvent["ID"].asString(), parsedEvent);
    return true;
  }
  return false;
}

// Print the event map
void printEvents() {
  for(const auto& [key, event] : eventMap) {
    std::cout << event << '\n';
  }
}

/****** NYSDOT::EVENT ******/

// Construct an event from a parsed Json Event
Event::Event(const Json::Value &parsedEvent)
: ID{ parsedEvent["ID"].asString() },
  RegionName{ parsedEvent["RegionName"].asString() },
  CountyName{ parsedEvent["CountyName"].asString() },
  Severity{ parsedEvent["Severity"].asString() },
  RoadwayName{ parsedEvent["RoadwayName"].asString() },
  DirectionOfTravel{ parsedEvent["DirectionOfTravel"].asString() },
  Description{ parsedEvent["Description"].asString() },
  Location{ parsedEvent["Location"].asString() },
  LanesAffected{ parsedEvent["LanesAffected"].asString() },
  LanesStatus{ parsedEvent["LanesStatus"].asString() },
  PrimaryLocation{ parsedEvent["PrimaryLocation"].asString() },
  SecondaryLocation{ parsedEvent["SecondaryLocation"].asString() },
  FirstArticleCity{ parsedEvent["FirstArticleCity"].asString() },
  SecondCity{ parsedEvent["SecondCity"].asString() },
  EventType{ parsedEvent["EventType"].asString() },
  EventSubType{ parsedEvent["EventSubType"].asString() },
  MapEncodedPolyline{ parsedEvent["MapEncodedPolyline"].asString() },
  LastUpdated{ parsedEvent["LastUpdated"].asString() },
  Latitude{ parsedEvent["Latitude"].asDouble() },
  Longitude{ parsedEvent["Longitude"].asDouble() },
  PlannedEndDate{ parsedEvent["PlannedEndDate"].asString() },
  Reported{ parsedEvent["Reported"].asString() },
  StartDate{ parsedEvent["StartDate"].asString() } 
{}

// Define the move constructor
Event::Event(Event&& other) noexcept 
: ID(std::move(other.ID)),
  RegionName(std::move(other.RegionName)),
  CountyName(std::move(other.CountyName)),
  Severity(std::move(other.Severity)),
  RoadwayName(std::move(other.RoadwayName)),
  DirectionOfTravel(std::move(other.DirectionOfTravel)),
  Description(std::move(other.Description)),
  Location(std::move(other.Location)),
  LanesAffected(std::move(other.LanesAffected)),
  LanesStatus(std::move(other.LanesStatus)),
  PrimaryLocation(std::move(other.PrimaryLocation)),
  SecondaryLocation(std::move(other.SecondaryLocation)),
  FirstArticleCity(std::move(other.FirstArticleCity)),
  SecondCity(std::move(other.SecondCity)),
  EventType(std::move(other.EventType)),
  EventSubType(std::move(other.EventSubType)),
  MapEncodedPolyline(std::move(other.MapEncodedPolyline)),
  LastUpdated(std::move(other.LastUpdated)),
  Latitude(other.Latitude), // primitives can be copied directly
  Longitude(other.Longitude),
  PlannedEndDate(std::move(other.PlannedEndDate)),
  Reported(std::move(other.Reported)),
  StartDate(std::move(other.StartDate))
{}

// Define the move assignment operator
Event& Event::operator=(Event&& other) noexcept {
  // Check for self assignment
  if (this != &other) {
    ID = std::move(other.ID);
    RegionName = std::move(other.RegionName);
    CountyName = std::move(other.CountyName);
    Severity = std::move(other.Severity);
    RoadwayName = std::move(other.RoadwayName);
    DirectionOfTravel = std::move(other.DirectionOfTravel);
    Description = std::move(other.Description);
    Location = std::move(other.Location);
    LanesAffected = std::move(other.LanesAffected);
    LanesStatus = std::move(other.LanesStatus);
    PrimaryLocation = std::move(other.PrimaryLocation);
    SecondaryLocation = std::move(other.SecondaryLocation);
    FirstArticleCity = std::move(other.FirstArticleCity);
    SecondCity = std::move(other.SecondCity);
    EventType = std::move(other.EventType);
    EventSubType = std::move(other.EventSubType);
    MapEncodedPolyline = std::move(other.MapEncodedPolyline);
    LastUpdated = std::move(other.LastUpdated);
    Latitude = other.Latitude; // primitive types can be directly copied
    Longitude = other.Longitude;
    PlannedEndDate = std::move(other.PlannedEndDate);
    Reported = std::move(other.Reported);
    StartDate = std::move(other.StartDate);
  }
  return *this;
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
} // namespace Traffic
