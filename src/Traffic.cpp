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

bool processEvent(const Json::Value& parsedEvent) {
  // Extract the event ID
  std::string eventID{ parsedEvent["ID"].asString() };

  /* TODO:
   * Check eventID against objects on the map
   * IF (eventMap.find(eventID) != eventMap.end() THEN
   *    Check LastUpdated to see if it has changed
   *    IF it has changed THEN
   *        eventMap[eventID].update(parsedEvent)
   * ELSE continue on below
   */

  // Extract the region
  std::string eventRegion{ parsedEvent["RegionName"].asString() };

  // Check against matching region(s)
  if(eventRegion == "Central Syracuse Utica Area") {

    // Construct an event object
    Event event(parsedEvent);
    
    /* TODO:
     * Move the constructed object onto the map
     */

    return true;
  }
  return false;
}

/****** NYSDOT::EVENT ******/

// Construct an event from a parsed Json Event
Event::Event(const Json::Value &parsedEvent)
: ID { parsedEvent["ID"].asString() } 
{
  update(parsedEvent);
  std::cout << "[NYSDOT] Constructed new event object\n";
}

// Update all data members
void Event::update(const Json::Value &parsedEvent) {
  RegionName = parsedEvent["RegionName"].asString();
  CountyName = parsedEvent["CountyName"].asString();
  Severity = parsedEvent["Severity"].asString();
  RoadwayName = parsedEvent["RoadwayName"].asString();
  DirectionOfTravel = parsedEvent["DirectionOfTravel"].asString();
  Description = parsedEvent["Description"].asString();
  Location = parsedEvent["Location"].asString();
  LanesAffected = parsedEvent["LanesAffected"].asString();
  LanesStatus = parsedEvent["LanesStatus"].asString();
  PrimaryLocation = parsedEvent["PrimaryLocation"].asString();
  SecondaryLocation = parsedEvent["SecondaryLocation"].asString();
  FirstArticleCity = parsedEvent["FirstArticleCity"].asString();
  SecondCity = parsedEvent["SecondCity"].asString();
  EventType = parsedEvent["EventType"].asString();
  EventSubType = parsedEvent["EventSubType"].asString();
  MapEncodedPolyline = parsedEvent["MapEncodedPolyline"].asString();
  LastUpdated = parsedEvent["LastUpdated"].asString();
  Latitude = parsedEvent["Latitude"].asDouble();
  Longitude = parsedEvent["Longitude"].asDouble();
  PlannedEndDate = parsedEvent["PlannedEndDate"].asString();
  Reported = parsedEvent["Reported"].asString();
  StartDate = parsedEvent["StartDate"].asString();
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
