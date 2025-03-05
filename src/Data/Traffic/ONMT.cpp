#include "Traffic/ONMT.h"
#include "DataUtils.h"
#include "Output.h"
#include <string>
#include <iostream>

namespace Traffic {
namespace Ontario {

TrafficMap<std::string, Event> eventMap; // Key = "ID"
TrafficMap<int, Camera> cameraMap; // Key = "ID"
constexpr BoundingBox regionToronto{ -80.099, -78.509, 44.205, 43.137 };

bool getEvents() {
  static const std::string url{ "https://511on.ca/api/v2/get/event?format=json&lang=en" };

  // Retrieve data from the URLm with cURL
  auto [result, responseStr] = cURL::getData(url);
  if(result == cURL::Result::SUCCESS) {
    // Make sure we received data
    if(responseStr.empty()) {
      std::cerr << Output::Colors::RED << "[cURL] Received empty response (no data).\n" << Output::Colors::END;
      return false;
    }

    std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved events JSON from Ontario 511.\n" << Output::Colors::END;

    // Test JSON parsing
    if(!parseEvents(JSON::parseData(responseStr))) {
      std::cerr << Output::Colors::RED << "[JSON] Error parsing events root tree.\n" << Output::Colors::END;
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

bool parseEvents(const Json::Value &events) {
  //Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check for validity of the object
    // TODO: This should be refactored into the processEvent function
    if(!parsedEvent.isObject()) {
      std::cerr << Output::Colors::RED << "[ONMT] Failed parsing event (is the JSON valid?)" << Output::Colors::END << '\n';
      return false; // Or do we want to continue here?
    }
    // Process the event for storage
    if(!processEvent(parsedEvent))
       return false;
  }
  std::cout << Output::Colors::GREEN << "\n[JSON] Successfully parsed events root tree." << Output::Colors::END << '\n';
  cleanEvents(events);
  std::cout << "[ONMT] Found " << eventMap.size() << " Matching Event Records.\n";

  return true;
}

bool processEvent(const Json::Value &parsedEvent) {
  std::string key = parsedEvent["ID"].asString();
  auto location = std::make_pair(parsedEvent["Latitude"].asDouble(), parsedEvent["Longitude"].asDouble());


  if(regionToronto.contains(location) && isIncident(parsedEvent)) {    // Check if bounding area contains the event location
    // Try to insert a new Event at event, inserted = false if it fails
    auto [event, inserted] = eventMap.try_emplace(key, parsedEvent);
    // Check if we added a new event
    if(!inserted) {
      // Check for change in event
      if(event->second.getLastUpdated() != parsedEvent["LastUpdated"].asInt()) {  // TODO: Should refactor into a less than check after implementing std::chrono
        event->second = parsedEvent;
        std::cout << Output::Colors::MAGENTA << "[ONMT] Updated event: " << key << Output::Colors::END << '\n';  
      }
    }
    // Check for valid event creation
    if(event->second.getLastUpdated() == 0)
      return false;
  }
return true;
}

// Clean up cleared events
void cleanEvents(const Json::Value& events) {
  // initialize a vector to store keys marked for deletion
  std::vector<std::string> keysToDelete;
  // Iterate through each event in the eventmap
  for(const auto& [key, event] : eventMap) {
    // Check if the event is found in the retrieved array
    if(!containsEvent(events, key)) {
      // Add the event's key to the deletion vector
      keysToDelete.push_back(key);
      std::cout << Output::Colors::YELLOW << "[ONMT] Marked event for deletion: " << key << Output::Colors::END << '\n'; 
    }
  }
  deleteEvents(keysToDelete);
}

// Check if yhe Json Array contains an event with the given key
bool containsEvent(const Json::Value& events, const std::string& key) {
  // Confirm the object is an array
  if(!events.isArray()) {
    std::cerr << Output::Colors::RED << "[ONMT] JSON not a valid array!\n" << Output::Colors::END;
  }

  // Iterate through the array and check for matching key
  for(const auto& parsedEvent : events) {
    // Confirm we parsed a valid object
    if(!parsedEvent.isObject() || !parsedEvent.isMember("ID")) {
      std::cerr << Output::Colors::RED << "[ONMT] Parsed JSON not a valid object!\n" << Output::Colors::END;
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

bool isIncident(const Json::Value& parsedEvent){
  std::string type = parsedEvent["EventType"].asString();
  return (type == "accidentsAndIncidents"
       || type == "closures");
}

bool isConstruction(const Json::Value& parsedEvent){
  return parsedEvent["EventType"].asString() == "roadwork";
}

// Delete all events with matching keys from. the deletion vector
void deleteEvents(const std::vector<std::string>& keys) {
  for(const auto& key : keys) {
    eventMap.erase(key);
    std::cout << Output::Colors::RED << "[ONMT] Deleted event: " << key << Output::Colors::END << '\n'; 
  }
}

bool getCameras() {
  // Build the request URL
  static const std::string url{ "https://511on.ca/api/v2/get/cameras?format=json&lang=en" };

  // Retrieve data from URL with cURL
  auto [result, responseStr] = cURL::getData(url);
  if(result == cURL::Result::SUCCESS){
    // Make sure we received data
    if(responseStr.empty()) {
      std::cerr << Output::Colors::RED << "[cURL] Received empty response (no data).\n" << Output::Colors::END;
      return false;
    }
    
    std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved cameras JSON from Ontario 511.\n" << Output::Colors::END;
    
    // Parse Cameras from Response String
    if(!parseCameras(JSON::parseData(responseStr))) {
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

bool parseCameras(const Json::Value &cameras) {
  for(const auto& parsedCamera : cameras) {
    if(!parsedCamera.isObject()) {
      std::cerr << Output::Colors::RED << "[ONMT] Failed parsing camera (is the JSON valid?)" << Output::Colors::END << '\n';
      return false; // Or do we want to continue here?
    }

    if(!processCamera(parsedCamera))
      return false;
  }
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed cameras root tree." << Output::Colors::END << '\n';
  std::cout << "[ONMT] Found " << cameraMap.size() << " Matching camera Records.\n";
  return true;
}

bool processCamera(const Json::Value &parsedCamera) {
  int key = parsedCamera["Id"].asInt();
  auto location = std::make_pair(parsedCamera["Latitude"].asDouble(), parsedCamera["Longitude"].asDouble());

  if(regionToronto.contains(location)) {
    auto [camera, inserted] = cameraMap.try_emplace(key, parsedCamera);
    if(!inserted) {
      // Check if camera status changed??
      camera->second = parsedCamera;
      std::cout << Output::Colors::MAGENTA << "[ONMT] Updated camera: " << key << Output::Colors::END << '\n';  
    }
  }
  return true;
}

/******* Ontario MT Events *********/
// Construct an event from a parsed Json object
Event::Event(const Json::Value& parsedEvent) {
  if(parsedEvent.isMember("ID"))
    ID = parsedEvent["ID"].asString();
  if(parsedEvent.isMember("Organization"))
    Organization = parsedEvent["Organization"].asString();
  if(parsedEvent.isMember("RoadwayName"))
    RoadwayName = parsedEvent["RoadwayName"].asString();
  if(parsedEvent.isMember("DirectionOfTravel"))
     DirectionOfTravel = parsedEvent["DirectionOfTravel"].asString();
  if(parsedEvent.isMember("Description"))
    Description = parsedEvent["Description"].asString();
  if(parsedEvent.isMember("Reported"))
    Reported = parsedEvent["Reported"].asInt();
  if(parsedEvent.isMember("LastUpdated"))
    LastUpdated = parsedEvent["LastUpdated"].asInt();
  if(parsedEvent.isMember("StartDate"))
    StartDate = parsedEvent["StartDate"].asInt();
  if(parsedEvent.isMember("PlannedEndDate"))
    PlannedEndDate = parsedEvent["PlannedEndDate"].asInt();
  if(parsedEvent.isMember("LanesAffected"))
    LanesAffected = parsedEvent["LanesAffected"].asString();
  if(parsedEvent.isMember("Latitude"))
    Latitude = parsedEvent["Latitude"].asDouble();
  if(parsedEvent.isMember("Longitude"))
    Longitude = parsedEvent["Longitude"].asDouble();
  if(parsedEvent.isMember("LatitudeSecondary"))
    LatitudeSecondary = parsedEvent["LatitudeSecondary"].asDouble();
  if(parsedEvent.isMember("LongitudeSecondary"))
    LongitudeSecondary = parsedEvent["LongitudeSecondary"].asDouble();
  if(parsedEvent.isMember("EventType"))
    EventType = parsedEvent["EventType"].asString();
  if(parsedEvent.isMember("IsFullClosure"))
    IsFullClosure = parsedEvent["IsFullClosure"].asBool();
  if(parsedEvent.isMember("Comment"))
    Comment = parsedEvent["Comment"].asString();
  if(parsedEvent.isMember("Recurrence"))
    Recurrence = parsedEvent["Recurrence"].asString();
  if(parsedEvent.isMember("RecurrenceSchedules"))
    RecurrenceSchedules = parsedEvent["RecurrenceSchedules"].asString();
  if(parsedEvent.isMember("EventSubType"))
    EventSubType = parsedEvent["EventSubType"].asString();
  if(parsedEvent.isMember("EncodedPolyline"))
    EncodedPolyline = parsedEvent["EncodedPolyline"].asString();
  if(parsedEvent.isMember("LinkId"))
    LinkId = parsedEvent["LinkId"].asString();

  std::cout << Output::Colors::YELLOW << "\n[ONMT] Constructed event: " << ID << Output::Colors::END 
            << '\n' << Description << '\n';
}

// Define the move constructor
Event::Event(Event&& other) noexcept
: ID(std::move(other.ID)),
  Organization(std::move(other.Organization)),
  RoadwayName(std::move(other.RoadwayName)),
  DirectionOfTravel(std::move(other.DirectionOfTravel)),
  Description(std::move(other.Description)),
  Reported(std::move(other.Reported)),
  LastUpdated(std::move(other.LastUpdated)),
  StartDate(std::move(other.StartDate)),
  PlannedEndDate(std::move(other.PlannedEndDate)),
  LanesAffected(std::move(other.LanesAffected)),
  Latitude(std::move(other.Latitude)),
  Longitude(std::move(other.Longitude)),
  LatitudeSecondary(std::move(other.LatitudeSecondary)),
  LongitudeSecondary(std::move(other.LongitudeSecondary)),
  EventType(std::move(other.EventType)),
  IsFullClosure(std::move(other.IsFullClosure)),
  Comment(std::move(other.Comment)),
  Recurrence(std::move(other.Recurrence)),
  RecurrenceSchedules(std::move(other.RecurrenceSchedules)),
  EventSubType(std::move(other.EventSubType)),
  EncodedPolyline(std::move(other.EncodedPolyline)),
  LinkId(std::move(other.LinkId))
{
  std::cout << Output::Colors::BLUE << "[ONMT] Moved event: " << ID << Output::Colors::END
            << '\n' << Description << '\n';
}
  
//Define the move assignment operator
Event& Event::operator=(Event&& other) noexcept {
  // Check for self assignment
  if (this != &other) {
    ID = std::move(other.ID);
    Organization = std::move(other.Organization);
    RoadwayName = std::move(other.RoadwayName);
    DirectionOfTravel = std::move(other.DirectionOfTravel);
    Description = std::move(other.Description);
    Reported = other.Reported;
    LastUpdated = other.LastUpdated;
    StartDate = other.StartDate;
    PlannedEndDate = other.PlannedEndDate;
    LanesAffected = std::move(other.LanesAffected);
    Latitude = other.Latitude; // primitive types can be directly copied
    Longitude = other.Longitude;
    LatitudeSecondary = other.LatitudeSecondary;
    LongitudeSecondary = other.LongitudeSecondary;
    EventType = std::move(other.EventType);
    IsFullClosure = other.IsFullClosure;
    Comment = std::move(other.Comment);
    Recurrence = std::move(other.Recurrence);
    RecurrenceSchedules = std::move(other.RecurrenceSchedules);
    EventSubType = std::move(other.EventSubType);
    EncodedPolyline = std::move(other.EncodedPolyline);
    LinkId = std::move(other.LinkId);
  }
  std::cout << Output::Colors::BLUE << "[ONMT] Invoked move assignment for event: " << ID << Output::Colors::END
            << '\n' << Description << '\n';
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Event &event) {
  out << "Ontario 511 Event:"
      << "\nID: " << event.ID
      << "\nLocation: " << event.RoadwayName << "  |  " << event.DirectionOfTravel
      << "\nDetails: " << event.Description
      << std::endl;
  return out;
}

/******* Ontario MT Cameras *********/
// Construct a camera from a parsed Json object
Camera::Camera(const Json::Value& parsedCamera) {
  if(parsedCamera.isMember("Id"))
    ID = parsedCamera["Id"].asInt();
  if(parsedCamera.isMember("Source"))
    Source = parsedCamera["Source"].asString();
  if(parsedCamera.isMember("SourceId"))
    SourceID = parsedCamera["SourceId"].asString();
  if(parsedCamera.isMember("Roadway"))
    Roadway = parsedCamera["Roadway"].asString();
  if(parsedCamera.isMember("Direction"))
    Direction = parsedCamera["Direction"].asString();
  if(parsedCamera.isMember("Latitude"))
    Latitude = parsedCamera["Latitude"].asDouble();
  if(parsedCamera.isMember("Longitude"))
    Longitude = parsedCamera["Longitude"].asDouble();
  if(parsedCamera.isMember("Location"))
    Location = parsedCamera["Location"].asString();
  if(parsedCamera.isMember("Views")){
    // Construct a CameraView object and push it on a vector of views
    const Json::Value views = parsedCamera["Views"];
    for (const auto& view : views){
      Views.emplace_back(view);
    }
    std::cout << Output::Colors::YELLOW << "[ONMT] Found " << Views.size() << " camera views for " << ID << Output::Colors::END << '\n';
  }
  std::cout << Output::Colors::YELLOW << "[ONMT] Constructed camera: " << ID << " | " << Views.at(0).getURL() << Output::Colors::END << '\n';
}

Camera::Camera(Camera&& other) noexcept
: ID(other.ID),
  Source(std::move(other.Source)),
  SourceID(std::move(other.SourceID)),
  Roadway(std::move(other.Roadway)),
  Direction(std::move(other.Direction)),
  Latitude(other.Latitude),
  Longitude(other.Longitude),
  Location(std::move(other.Location)),
  Views(std::move(other.Views))
{
  std::cout << Output::Colors::BLUE << "[ONMT] Moved camera: " << ID << Output::Colors::END << '\n';
}
  
Camera& Camera::operator=(Camera&& other) noexcept {
  if(this != &other){
    ID = other.ID;
    Source = std::move(other.Source);
    SourceID = std::move(other.SourceID);
    Roadway = std::move(other.Roadway);
    Direction = std::move(other.Direction);
    Latitude = other.Latitude;
    Longitude = other.Longitude;
    Location = std::move(other.Location);
    Views = std::move(other.Views);
  }
  std::cout << Output::Colors::BLUE << "[ONMT] Invoked move assignment for camera: " << ID << Output::Colors::END << '\n';
  return *this;
}

CameraView::CameraView(const Json::Value& parsedView) {
  if(parsedView.isMember("Id"))
    ID = parsedView["Id"].asInt();
  if(parsedView.isMember("Url"))
    URL = parsedView["Url"].asString();
  if(parsedView.isMember("Status"))
    Status = parsedView["Status"].asString();
  if(parsedView.isMember("Description"))
    Description = parsedView["Description"].asString();
}
} // namespace Ontario
} // namespace Traffic