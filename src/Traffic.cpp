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
    eventMap.insert_or_assign(eventID, std::move(event));
    
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
{
  std::cout << "[NYSDOT::EVENT] Invoked the move constructor.\n";
}

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
  std::cout << "[NYSDOT::EVENT] Invoked move assignment.\n";
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





/************************ Monroe County Dispatch Feed *************************/

namespace MCNY {

} // namespace MCNY
} // namespace Traffic
