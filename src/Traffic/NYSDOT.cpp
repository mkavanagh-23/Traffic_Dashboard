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
 *  - Multithreading and loop structure - updates in the backend
 *  - Logging
 */

namespace NYSDOT {

std::string API_KEY;
TrafficMap<std::string, Event> eventMap; // Key = "ID"
TrafficMap<std::string, Camera> cameraMap; // Key = "ID"
constexpr BoundingBox regionSyracuse{ -76.562, -75.606, 43.553, 42.621 };

bool getEnv() {
  // Retrieve API Key from local environment
  const char* NYSDOT_API_KEY = std::getenv("NYSDOT_API_KEY");

  // Check if environment variable exists
  if(NYSDOT_API_KEY) {
    API_KEY = NYSDOT_API_KEY;    
    std::cout << Output::Colors::GREEN << "[ENV] Successfully sourced API key from local environment.\n" << Output::Colors::END;
    return true;
  } else {
    std::cerr << Output::Colors::RED << "[ENV] Failed to retrieve 'NYSDOT_API_KEY'.\nBe sure you have it set.\n" << Output::Colors::END;
    return false;
  }
}

bool getEvents(){
  // Build the request URL
  std::string url{ "https://511ny.org/api/getevents?format=json&key=" + API_KEY };

  // Retrieve data from URL with cURL
  auto [result, responseStr] = cURL::getData(url);
  if(result == cURL::Result::SUCCESS) {
    // Make sure we received data
    if(responseStr.empty()) {
      std::cerr << Output::Colors::RED << "[cURL] Received empty response (no data).\n" << Output::Colors::END;
      return false;
    }

    std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved events JSON from 511ny.\n" << Output::Colors::END;

    // Test JSON Parsing
    if(!parseEvents(JSON::parseData(responseStr))) {
      std::cerr << Output::Colors::RED << "[JSON] Error parsing root tree.\n" << Output::Colors::END;
      return false;
    }

  } else {
    // Handle the error
    switch(result) {
      case cURL::Result::TIMEOUT:
        std::cerr << Output::Colors::RED << "[cURL] Timed out retrieving data from remote stream. Retrying in 60 seconds..." << Output::Colors::END;
        break;
      default:
        std::cerr << Output::Colors::RED << "[cURL] Critical error retrieiving data from remote stream. Terminating program." << Output::Colors::END;
        return false;
    }
  }
  return true;
}

// Parse events from the root events object
bool parseEvents(const Json::Value& events){
  // Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check if event is a valid object
    if(!parsedEvent.isObject()) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing event (is the JSON valid?)\n" << Output::Colors::END;
      return false;
    }
    // Process the event for storage
    processEvent(parsedEvent);
  }
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed root tree.\n" << Output::Colors::END;
  // Clean up any cleared events
  cleanEvents(events);
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

// Clean up cleared events
void cleanEvents(const Json::Value& events) {
  std::vector<std::string> keysToDelete;
  // Iterate through each item in the map
  for(const auto& [key, event] : eventMap) { // NOTE: Be extra mindful of iterator invalidation (SEE BELOW)

    // Check if a match was found in the Json::Value object
    if(!containsEvent(events, key)) {
      // If not match is found, the event was cleared so delete it from the map
      //eventMap.erase(key);      
                                // FIX: Cannot do this here as we are currently iterating through the eventMap object
                                // This means that when we delete the keyed object we invalidate our iterator leading to UB
                                // Perhaps instead of erasing in the loop we could create a vector of keys, and then afterward we can delete each matching Event values
                                // This is TOP PRIORITY and should be studied as it is a common pitfall of working with hashmaps
      
      // If no matching event, add it to the deletion vector
      keysToDelete.push_back(key);

      std::cout << Output::Colors::YELLOW << "[NYSDOT] Marked event for deletion: " << key << Output::Colors::END << '\n'; 
    }
  }
  deleteEvents(keysToDelete);
}

// Check if the Json array contains an event with the given key
// TODO: MUST REFACTOR VALIDITY CHECKS INTO TRY-CATCH
bool containsEvent(const Json::Value& events, const std::string& key) {
  // Confirm object is array
  if(!events.isArray()) {
    std::cerr << Output::Colors::RED << "[NYSDOT] JSON not a valid array!\n" << Output::Colors::END;
  }

  // Iterate through the array and check for matching key
  for(const auto& parsedEvent : events) {
    // Check for a valid object
    if(!parsedEvent.isObject() || !parsedEvent.isMember("ID")) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Parsed JSON not a valid object!\n" << Output::Colors::END;
      continue;
    }
    // Check for a key match
    if(parsedEvent["ID"].asString() == key)
      return true;
    else
      continue;
  }
  return false;
}

void deleteEvents(const std::vector<std::string>& keys) {
  for(const auto& key : keys) {
    eventMap.erase(key);
    std::cout << Output::Colors::RED << "[NYSDOT] Deleted event: " << key << Output::Colors::END << '\n'; 
  }
}

// Print the event map
void printEvents() {
  for(const auto& [key, event] : eventMap) {
    std::cout << event << '\n';
  }
}

bool getCameras() {
  // Build the request URL
  std::string url{ "https://511ny.org/api/getcameras?format=json&key=" + API_KEY };
  
  // Retrieve data from URL with cURL
  auto [result, responseStr] = cURL::getData(url);
  if(result == cURL::Result::SUCCESS){
    // Make sure we received data
    if(responseStr.empty()) {
      std::cerr << Output::Colors::RED << "[cURL] Received empty response (no data).\n" << Output::Colors::END;
      return false;
    }
    
    std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved cameras JSON from NYSDOT.\n" << Output::Colors::END;

    // Parse Events Data from API
    if(!parseCameras(JSON::parseData(responseStr))){
      std::cerr << Output::Colors::RED << "[JSON] Error parsing cameras root tree.\n" << Output::Colors::END;
      return false;
    }
  } else {
    // Handle the error
    switch(result) {
      case cURL::Result::TIMEOUT:
        std::cerr << Output::Colors::RED << "[cURL] Timed out retrieving data from remote stream. Retrying in 10 minutes." << Output::Colors::END;
        break;
      default:
        std::cerr << Output::Colors::RED << "[cURL] Critical error retrieiving data from remote stream. Terminating program." << Output::Colors::END;
        return false;
    }
  }
  return true;
}

bool parseCameras(const Json::Value &cameras){
  for(const auto& parsedCamera : cameras) {
    if(!parsedCamera.isObject()) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing camera (is the JSON valid?)\n" << Output::Colors::END;
      return false; // Or do we want to continue here?
    }
    processCamera(parsedCamera);
  }

  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed cameras root tree." << Output::Colors::END << '\n';
  std::cout << "[NYSDOT] Found " << cameraMap.size() << " Matching camera Records.\n";
  return true;
}

bool processCamera(const Json::Value &parsedCamera){
  std::string key = parsedCamera["ID"].asString();
  auto location = std::make_pair(parsedCamera["Latitude"].asDouble(), parsedCamera["Longitude"].asDouble());
  // Make sure camera is not disabled or blocked
  if(!(parsedCamera["Disabled"].asBool()) && !(parsedCamera["Blocked"].asBool())) {
    // Check for our region
    if(regionSyracuse.contains(location)) {
      auto [camera, inserted] = cameraMap.try_emplace(key, parsedCamera);
      if(!inserted) {
        // Check if camera status changed??
        camera->second = parsedCamera;
        std::cout << Output::Colors::MAGENTA << "[NYSDOT] Updated camera: " << key << Output::Colors::END << '\n';  
      }
    }
  }
  return true;
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


/****** NYSDOT::Camera ******/
// Construct a camera from a parsed Json object
Camera::Camera(const Json::Value& parsedCamera) {
  if(parsedCamera.find("ID"))
    ID = parsedCamera["ID"].asString();
  if(parsedCamera.find("Url"))
    URL = parsedCamera["Url"].asString();
  if(parsedCamera.find("VideoUrl"))
    VideoURL = parsedCamera["VideoUrl"].asString();
  if(parsedCamera.find("Name"))
    Name = parsedCamera["Name"].asString();
  if(parsedCamera.find("DirectionOfTravel"))
    DirectionOfTravel = parsedCamera["DirectionOfTravel"].asString();
  if(parsedCamera.find("RoadwayName"))
    RoadwayName = parsedCamera["RoadwayName"].asString();
  if(parsedCamera.find("Disabled"))
    Disabled = parsedCamera["Disabled"].asBool();
  if(parsedCamera.find("Blocked"))
    Blocked = parsedCamera["Blocked"].asBool();
  if(parsedCamera.find("Latitude"))
    Latitude = parsedCamera["Latitude"].asDouble();
  if(parsedCamera.find("Longitude"))
    Longitude = parsedCamera["Longitude"].asDouble();

  std::cout << Output::Colors::YELLOW << "[NYSDOT] Constructed camera: " << ID << " | " << VideoURL << Output::Colors::END << '\n';
}

Camera::Camera(Camera&& other) noexcept
: ID(std::move(other.ID)),
  URL(std::move(other.URL)),
  VideoURL(std::move(other.VideoURL)),
  Name(std::move(other.Name)),
  DirectionOfTravel(std::move(other.DirectionOfTravel)),
  RoadwayName(std::move(other.RoadwayName)),
  Disabled(other.Disabled),
  Blocked(other.Blocked),
  Latitude(other.Latitude),
  Longitude(other.Longitude)
{
  std::cout << Output::Colors::BLUE << "[NYSDOT] Moved camera: " << ID << Output::Colors::END << '\n';
}

Camera& Camera::operator=(Camera&& other) noexcept {
  if(this != &other){
    ID = other.ID;
    URL = std::move(other.URL);
    VideoURL = std::move(other.VideoURL);
    Name = std::move(other.Name);
    DirectionOfTravel = std::move(other.DirectionOfTravel);
    RoadwayName = std::move(other.RoadwayName);
    Disabled = other.Disabled;
    Blocked = other.Blocked;
    Latitude = other.Latitude;
    Longitude = other.Longitude;
  }
  std::cout << Output::Colors::BLUE << "[NYSDOT] Invoked move assignment for camera: " << ID << Output::Colors::END << '\n';
  return *this;
}
} // namespace NYSDOT
}
