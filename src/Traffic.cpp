#include "Traffic.h"
#include "Data.h"

#include <string>
#include <iostream>
#include <dotenv.h>
#include <rapidxml.hpp>

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

bool getEvents(){
  // Source API key from local environment
  // This should eventually be moved into some kind of setup() function to make sure it executes only once and before the main loop begins
  API_KEY = std::getenv("NYSDOT_API_KEY");
  if(API_KEY.empty()) {
    std::cerr << "\033[31m[dotEnv] Failed to retrieve 'NYSDOT_API_KEY'.\nBe sure you have defined it in '.env'.\033[0m\n";
    return false;
  }
  std::cout << "\033[32m[dotEnv] Successfully sourced API key from local environment.\033[0m\n";
  
  // Parse Events Data from API
  std::string url{ "https://511ny.org/api/getevents/?format=json&key=" + API_KEY };
  std::string responseStr{ cURL::getData(url) };
  if(responseStr.empty()) {
    std::cerr << "\033[31m[cURL] Failed to retrieve JSON from 511ny.\033[0m\n";
    return false;
  }
  std::cout << "\033[32m[cURL] Successfully retrieved JSON from 511ny.\033[0m\n";
  
  // Test JSON Parsing
  if(!parseEvents(JSON::parseData(responseStr))) {
    std::cerr << "\033[31m[EVENT] Error parsing root tree.\033[0m\n";
    return false;
  }
  std::cout << "\033[32m[EVENT] Successfully parsed root tree.\033[0m\n";

  return true;
}

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
  std::cout << "Found " << eventMap.size() << " Matching Event Records.\n";
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
{
  std::cout << "Constructed NYSDOT event: " << ID << '\n';
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
  std::cout << "Moved NYSDOT event: " << ID << '\n';
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
  std::cout << "[NYSDOT] Invoked move assignment: " << ID << '\n';
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

/************************ Monroe County Dispatch Feed *************************/

namespace MCNY {
const std::string RSS_URL{ "https://www.monroecounty.gov/incidents911.rss" };
EventMap<Event> eventMap; // Key = "ID"

bool getEvents() {
  // Parse Events Data from RSS feed
  std::string responseStr{ cURL::getData(RSS_URL) };
  if(responseStr.empty()) {
    std::cerr << "\033[31m[cURL] Failed to retrieve XML from RSS feed.\033[0m\n";
    return false;
  }
  std::cout << "\033[32m[cURL] Successfully retrieved XML from RSS feed.\033[0m\n";

  // Test XML parsing
  rapidxml::xml_document<> parsedData;  // Create a document object to hold XML events
  XML::parseData(parsedData, responseStr);
  if(!parseEvents(parsedData)) {
    std::cerr << "\033[31m[XML] Error parsing root tree.\033[0m\n";
    return false;
  }
  std::cout << "\033[32m[XML] Successfully parsed root tree.\033[0m\n";

  return true;
}

bool parseEvents(rapidxml::xml_document<>& xml) { 
  rapidxml::xml_node<>* root = xml.first_node("rss"); // Define root entry point
  rapidxml::xml_node<>* channel = root->first_node("channel"); // Navigate to channel
  
  // Iterate throgh each event in the document tree
  for(rapidxml::xml_node<>* item = channel->first_node("item"); item; item = item->next_sibling()) {
    // Create a temporary event object
    Event event(item);
    eventMap.insert_or_assign(event.getID(), std::move(event));
    // TODO:
    // Log time to track LastUpdated
  }
  std::cout << "Found " << eventMap.size() << " Matching Event Records.\n";

  return true;
}

// Print the event map
void printEvents() {
  for(const auto& [key, event] : eventMap) {
    std::cout << event << '\n';
  }
}

/***************************** MCNY EVENT *************************************/
//Construct an event from a parsed XML item

Event::Event(const rapidxml::xml_node<>* item) {
  if(rapidxml::xml_node<> *title = item->first_node("title")) {
    Title = title->value();
  }
  if(rapidxml::xml_node<> *link = item->first_node("link")) {
    Link = link->value();
  }
  if(rapidxml::xml_node<> *pubDate = item->first_node("pubDate")) {
    PubDate = pubDate->value();
  }
  if(rapidxml::xml_node<> *guid = item->first_node("guid")) {
    GUID = guid->value();
  }
  if(rapidxml::xml_node<> *latitude = item->first_node("geo:lat")) {
    std::string temp = latitude->value();
    Latitude = std::stof(temp.substr(1));
  }
  if(rapidxml::xml_node<> *longitude = item->first_node("geo:long")) {
    std::string temp = longitude->value();
    Longitude = std::stof(temp.substr(1));
  }
  // Get the Status and ID
  if(rapidxml::xml_node<> *description = item->first_node("description")) {
    std::string temp;
    temp = description->value();
    
    // Extract items from the description
    std::stringstream ss(temp);
    std::string token;
    std::vector<std::string> tokens;
    
    // Elements delimited by ','
    while(std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }
    
    // First token is status
    Status = tokens[0].substr(tokens[0].find(":") + 2);
    // Second token is ID
    ID = tokens[1].substr(tokens[1].find(":") + 2);
  }
    
  std::cout << "Constructed MCNY event: " << ID << '\n';
}

  // Move constructor
Event::Event(Event&& other) noexcept
: ID(std::move(other.ID)),
  Title(std::move(other.Title)),
  Link(std::move(other.Link)),
  PubDate(std::move(other.PubDate)),
  Status(std::move(other.Status)),
  GUID(std::move(other.GUID)),
  Latitude(other.Latitude),
  Longitude(other.Longitude)
{
  std::cout << "Moved MCNY event: " << ID << '\n';
}

  // Move assignment operator
Event& Event::operator=(Event&& other) noexcept {
  // Check for self assignment
  if (this != &other) {
    ID = std::move(other.ID);
    Title = std::move(other.Title);
    Link = std::move(other.Link);
    PubDate = std::move(other.PubDate);
    Status = std::move(other.Status);
    GUID = std::move(other.GUID);
    Latitude = other.Latitude;
    Longitude = other.Longitude;
  }
  std::cout << "[MCNY] Invoked move assignment: " << '\n';
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Event &event) {
  out << "\nEvent ID: " << event.ID << "  |  " << event.Status
      << '\n' << event.Title
      << "\nDate: " << event.PubDate
      << std::endl;
  return out;
}
} // namespace MCNY
} // namespace Traffic
