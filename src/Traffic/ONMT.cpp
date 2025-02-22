#include "Traffic/ONMT.h"
#include "Data.h"
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
  
  // Parse events from API
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to retrieve events JSON from Ontario 511." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved events JSON from Ontario 511." << Output::Colors::END << '\n';

  // Test JSON parsing
  if(!parseEvents(JSON::parseData(responseStr))) {
    std::cerr << Output::Colors::RED << "[JSON] Error parsing events root tree." << Output::Colors::END << '\n';
    return false; 
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
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed events root tree." << Output::Colors::END << '\n';
  std::cout << "[ONMT] Found " << eventMap.size() << " Matching Event Records.\n";

  return true;
}

bool processEvent(const Json::Value &parsedEvent) {
  std::string key = parsedEvent["ID"].asString();
  auto location = std::make_pair(parsedEvent["Latitude"].asDouble(), parsedEvent["Longitude"].asDouble());


  if(regionToronto.contains(location)) {    // Check if bounding area contains the event location
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

bool getCameras() {
  // Build the request URL
  static const std::string url{ "https://511on.ca/api/v2/get/cameras?format=json&lang=en" };
  
  // Parse Events Data from API
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to retrieve cameras JSON from Ontario 511." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved cameras JSON from Ontario 511." << Output::Colors::END << '\n';

  if(!parseCameras(JSON::parseData(responseStr))) {
    std::cerr << Output::Colors::RED << "[JSON] Error parsing cameras root tree." << Output::Colors::END << '\n';
    return false;
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
      
      //TEST CODE
      camera->second = parsedCamera;
      std::cout << Output::Colors::MAGENTA << "[ONMT] Updated camera: " << key << Output::Colors::END << '\n';  
    }
  }
  return true;
}

/******* Ontario MT Events *********/
// Construct an event from a parsed Json object
Event::Event(const Json::Value& parsedEvent) {
  if(parsedEvent.find("ID"))
    ID = parsedEvent["ID"].asString();
  if(parsedEvent.find("Organization"))
    Organization = parsedEvent["Organization"].asString();
  if(parsedEvent.find("RoadwayName"))
    RoadwayName = parsedEvent["RoadwayName"].asString();
  if(parsedEvent.find("DirectionOfTravel"))
     DirectionOfTravel = parsedEvent["DirectionOfTravel"].asString();
  if(parsedEvent.find("Description"))
    Description = parsedEvent["Description"].asString();
  if(parsedEvent.find("Reported"))
    Reported = parsedEvent["Reported"].asInt();
  if(parsedEvent.find("LastUpdated"))
    LastUpdated = parsedEvent["LastUpdated"].asInt();
  if(parsedEvent.find("StartDate"))
    StartDate = parsedEvent["StartDate"].asInt();
  if(parsedEvent.find("PlannedEndDate"))
    PlannedEndDate = parsedEvent["PlannedEndDate"].asInt();
  if(parsedEvent.find("LanesAffected"))
    LanesAffected = parsedEvent["LanesAffected"].asString();
  if(parsedEvent.find("Latitude"))
    Latitude = parsedEvent["Latitude"].asDouble();
  if(parsedEvent.find("Longitude"))
    Longitude = parsedEvent["Longitude"].asDouble();
  if(parsedEvent.find("LatitudeSecondary"))
    LatitudeSecondary = parsedEvent["LatitudeSecondary"].asDouble();
  if(parsedEvent.find("LongitudeSecondary"))
    LongitudeSecondary = parsedEvent["LongitudeSecondary"].asDouble();
  if(parsedEvent.find("EventType"))
    EventType = parsedEvent["EventType"].asString();
  if(parsedEvent.find("IsFullClosure"))
    IsFullClosure = parsedEvent["IsFullClosure"].asBool();
  if(parsedEvent.find("Comment"))
    Comment = parsedEvent["Comment"].asString();
  if(parsedEvent.find("Recurrence"))
    Recurrence = parsedEvent["Recurrence"].asString();
  if(parsedEvent.find("RecurrenceSchedules"))
    RecurrenceSchedules = parsedEvent["RecurrenceSchedules"].asString();
  if(parsedEvent.find("EventSubType"))
    EventSubType = parsedEvent["EventSubType"].asString();
  if(parsedEvent.find("EncodedPolyline"))
    EncodedPolyline = parsedEvent["EncodedPolyline"].asString();
  if(parsedEvent.find("LinkId"))
    LinkId = parsedEvent["LinkId"].asString();

  std::cout << Output::Colors::YELLOW << "[ONMT] Constructed event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "[ONMT] Moved event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "[ONMT] Invoked move assignment for event: " << ID << Output::Colors::END << '\n';
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
  if(parsedCamera.find("Id"))
    ID = parsedCamera["Id"].asInt();
  if(parsedCamera.find("Source"))
    Source = parsedCamera["Source"].asString();
  if(parsedCamera.find("SourceId"))
    SourceID = parsedCamera["SourceId"].asString();
  if(parsedCamera.find("Roadway"))
    Roadway = parsedCamera["Roadway"].asString();
  if(parsedCamera.find("Direction"))
    Direction = parsedCamera["Direction"].asString();
  if(parsedCamera.find("Latitude"))
    Latitude = parsedCamera["Latitude"].asDouble();
  if(parsedCamera.find("Longitude"))
    Longitude = parsedCamera["Longitude"].asDouble();
  if(parsedCamera.find("Location"))
    Location = parsedCamera["Location"].asString();
  if(parsedCamera.find("Views")){
    // Construct a CameraView object and push it on a vector of views
    const Json::Value views = parsedCamera["Views"];
    for (const auto& view : views){
      Views.emplace_back(view);
    }
    std::cout << Output::Colors::YELLOW << "[ONMT] Found " << Views.size() << " camera views for " << ID << Output::Colors::END << '\n';
  }
  std::cout << Output::Colors::YELLOW << "[ONMT] Constructed camera: " << ID << Output::Colors::END << '\n';
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
  if(parsedView.find("Id"))
    ID = parsedView["Id"].asInt();
  if(parsedView.find("Url"))
    URL = parsedView["Url"].asString();
  if(parsedView.find("Status"))
    Status = parsedView["Status"].asString();
  if(parsedView.find("Description"))
    Description = parsedView["Description"].asString();
}
} // namespace Ontario
} // namespace Traffic
