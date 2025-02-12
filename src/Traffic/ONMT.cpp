#include "Traffic/ONMT.h"
#include "Data.h"
#include "Output.h"
#include <string>
#include <iostream>

namespace Traffic {
namespace Ontario {

EventMap<Event> eventMap; // Key = "ID"
//

bool getEvents() {
  std::string url{ "https://511on.ca/api/v2/get/event" };
  
  // Parse events from API
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to retrieve JSON from Ontario 511." << Output::Colors::END << std::endl;
    return false;
  }
  std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved JSON from 511ny." << Output::Colors::END << '\n';

  // Test JSON parsing
  if(!parseEvents(JSON::parseData(responseStr))) {
    std::cerr << Output::Colors::RED << "[ON EVENT] Error parsing root tree." << Output::Colors::END << '\n';
    return false; 
  }

  std::cout << Output::Colors::GREEN << "[ON EVENT] Successfully parsed root tree." << Output::Colors::END << '\n';
  return true;
}

bool parseEvents(const Json::Value &events) {
  //Iterate through each event
  for(const auto& parsedEvent : events) {
    // Check for validity of the object
    if(!parsedEvent.isObject()) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing event (is the JSON valid?)" << Output::Colors::END << '\n';
      return false;
    }
    // Process the event for storage
    if(!processEvent(parsedEvent))
       return false;
  }
  std::cout << "Found " << eventMap.size() << " Matching Event Records.\n";

  return true;
}

bool processEvent(const Json::Value &parsedEvent) {
  std::string key = parsedEvent["ID"].asString();
  if(eventMap.contains(key)) {
    if(eventMap.at(key).getLastUpdated() != parsedEvent["LastUpdated"].asInt()) {
      eventMap.at(key) = parsedEvent;
      std::cout << Output::Colors::YELLOW << "[NYSDOT] Updated event: " << key << Output::Colors::END << '\n';
    }
    return true;
  } else {
    eventMap.emplace(key, parsedEvent);  // Construct object in place
    return true;
  }
  return false;
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

  std::cout << Output::Colors::YELLOW << "Constructed Ontario 511 event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "Moved Ontario 511 event: " << ID << Output::Colors::END << '\n';
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
  std::cout << Output::Colors::BLUE << "[Ontario] Invoked move assignment: " << ID << Output::Colors::END << '\n';
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Event &event) {
  out << "Ontario 511 Event:"
      << "\nID: " << event.ID
      << "\nLocation: " << event.RoadwayName << "  |  " << event.DirectionOfTravel
      << "\nDetails: " << event.Description
      << "\n  " << event.Comment;
  return out;
}

} // namespace Ontario
} // namespace Traffic
