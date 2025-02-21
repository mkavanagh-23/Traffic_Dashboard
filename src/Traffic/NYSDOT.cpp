#include "Traffic/NYSDOT.h"
#include "Data.h"
#include "Output.h"
#include <string>
#include <iostream>
#include <cstdlib>

namespace Traffic {
/********************* NYSDOT Traffic Data (511ny.org) ************************/
/* TODO:
 *  - Clean up events once they are cleared
 *  - Error handling - try/catch
 *  - Get camera data
 *  - Multithreading and loop structure - updates in the backend
 *  - Logging
 */

namespace NYSDOT {

std::string API_KEY;
EventMap<Event> eventMap; // Key = "ID"

bool getEnv() {
  // Retrieve environment variable from local environment
  const char* NYSDOT_API_KEY = std::getenv("NYSDOT_API_KEY");

  // Check if environment variable exists
  if(NYSDOT_API_KEY) {
    API_KEY = NYSDOT_API_KEY;    
    std::cout << Output::Colors::GREEN << "[ENV] Successfully sourced API key from local environment." << Output::Colors::END << '\n';
    return true;
  } else {
    std::cerr << Output::Colors::RED << "[ENV] Failed to retrieve 'NYSDOT_API_KEY'.\nBe sure you have it set." << Output::Colors::END << '\n';
    return false;
  }
}

bool getEvents(){
  // Build the request URL
  std::string url{ "https://511ny.org/api/getevents/?format=json&key=" + API_KEY };

  // Parse Events Data from API
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to retrieve JSON from 511ny." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved JSON from 511ny." << Output::Colors::END << '\n';
  
  // Test JSON Parsing
  if(!parseEvents(JSON::parseData(responseStr))) {
    std::cerr << Output::Colors::RED << "[JSON] Error parsing root tree." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed root tree." << Output::Colors::END << '\n';

  return true;
}

// Parse events from the root events object
bool parseEvents(const Json::Value& events){
  // Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check if event is a valid object
    if(!parsedEvent.isObject()) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing event (is the JSON valid?)" << Output::Colors::END << '\n';
      return false;
    }
    // Process the event for storage
    processEvent(parsedEvent);
  }
  std::cout << "[NYSDOT] Found " << eventMap.size() << " Matching Event Records.\n";
  return true;
}

// Check the event against filter value and store on the map
bool processEvent(const Json::Value& parsedEvent) {
  std::string key = parsedEvent["ID"].asString();

  if( parsedEvent["RegionName"].asString()  == "Central Syracuse Utica Area") {
    // Try to insert a new Event at event, inserted = false if i already exists
    auto [event, inserted] = eventMap.try_emplace(key, parsedEvent);    // Access the event via structured bindings
    // Check if we added a new event
    if(!inserted) {
      //Check for an update 
      if(event->second.getLastUpdated() != parsedEvent["LastUpdated"].asString()) {
        event->second = parsedEvent;
        std::cout << Output::Colors::MAGENTA << "[NYSDOT] Updated event: " << key << Output::Colors::END << '\n';
      }
    }

    // Check for valid event creation
    if(event->second.getLastUpdated().empty())
      return false;
  }
  return true;
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
{
  if(parsedEvent.find("ID"))
    ID = parsedEvent["ID"].asString();
  if(parsedEvent.find("RegionName"))
    RegionName = parsedEvent["RegionName"].asString();
  if(parsedEvent.find("CountyName"))
    CountyName = parsedEvent["CountyName"].asString();
  if(parsedEvent.find("Severity"))
    Severity = parsedEvent["Severity"].asString();
  if(parsedEvent.find("RoadwayName"))
   RoadwayName = parsedEvent["RoadwayName"].asString();
  if(parsedEvent.find("DirectionOfTravel"))
    DirectionOfTravel = parsedEvent["DirectionOfTravel"].asString();
  if(parsedEvent.find("Description"))
    Description = parsedEvent["Description"].asString();
  if(parsedEvent.find("Location"))
    Location = parsedEvent["Location"].asString();
  if(parsedEvent.find("LanesAffected"))
    LanesAffected = parsedEvent["LanesAffected"].asString();
  if(parsedEvent.find("LanesStatus"))
    LanesStatus = parsedEvent["LanesStatus"].asString();
  if(parsedEvent.find("PrimaryLocation"))
    PrimaryLocation = parsedEvent["PrimaryLocation"].asString();
  if(parsedEvent.find("SecondaryLocation"))
    SecondaryLocation = parsedEvent["SecondaryLocation"].asString();
  if(parsedEvent.find("FirstArticleCity"))
    FirstArticleCity = parsedEvent["FirstArticleCity"].asString();
  if(parsedEvent.find("SecondCity"))
    SecondCity = parsedEvent["SecondCity"].asString();
  if(parsedEvent.find("EventType"))
    EventType = parsedEvent["EventType"].asString();
  if(parsedEvent.find("EventSubType"))
    EventSubType = parsedEvent["EventSubType"].asString();
  if(parsedEvent.find("MapEncodedPolyline"))
    MapEncodedPolyline = parsedEvent["MapEncodedPolyline"].asString();
  if(parsedEvent.find("LastUpdated"))
    LastUpdated = parsedEvent["LastUpdated"].asString();
  if(parsedEvent.find("Latitude"))
    Latitude = parsedEvent["Latitude"].asDouble();
  if(parsedEvent.find("Longitude"))
    Longitude = parsedEvent["Longitude"].asDouble();
  if(parsedEvent.find("PlannedEndDate"))
    PlannedEndDate = parsedEvent["PlannedEndDate"].asString();
  if(parsedEvent.find("Reported"))
    Reported = parsedEvent["Reported"].asString();
  if(parsedEvent.find("StartDate"))
    StartDate = parsedEvent["StartDate"].asString();

  std::cout << Output::Colors::YELLOW << "[NYSDOT] Constructed event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "[NYSDOT] Moved event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "[NYSDOT] Invoked move assignment: " << ID << Output::Colors::END << '\n';
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
      << "\nLast Updated: " << event.LastUpdated
      << std::endl;
  return out;
}

} // namespace NYSDOT
}
