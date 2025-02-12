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

  std::cout << Output::Colors::YELLOW << "Constructed Ontario 511 event: " << ID << Output::Colors::END << '\n';
}

} // namespace Ontario
} // namespace Traffic
