#include "Traffic.h"
#include "NYSDOT.h"
#include "MCNY.h"
#include "ONMT.h"
#include "MTL.h"
#include "OTT.h"
#include "ONGOV.h"
#include "DataUtils.h"
#include "Output.h"
#include "rapidxml.hpp"
#include <chrono>
#include <optional>
#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <unordered_map>

/* 
 TODO:
 *
 * ONGOV:
 *  Investigate REST API
 *    Possible workarounds for geo-restriction
 *    Do we need to proxy requests locally on client?
 *  Add logic to account for multiple pages
 *    Probably need to modify to POST request and investigate payloads in-browser
 *    Do we need to establish a session and maintain state?
 *    Modify cleanup to only run if we get valid table data
 *  Create an async function which can be run every few minutes to set geo-coordinates
 *    If we use openstreetmap we are limited to one request per second.
 *    Setup an atomic timer!
 *
 * ONMT: 
 *  Normalize data for Ontario events
 *    Need to extract optional side road if it exists, rest seems to be parsing fine
 *    Check reported time against current time to filter out future (planned) events
 *
 * OTT: 
 *  Add logic for Ottawa events
 *    Returns JSON
 *    Filter by "eventType" - We want "INCIDENT"
 *    brief overview in "headline"
 *
 * MTL: 
 *  Add logic for montreal events
 *    Returns XML
 *    Filter by <category> - we want "Warning"
 *    Use REGEX to parse description
 *  
 * Serializing to JSON:
 *  We should serialize time to an ISO6801-formatted string
 *  Serve serialized JSON via RESTful endpoints
 *  Implement robust filtering via query params
 */

namespace Traffic {

// Data structures
std::unordered_map<std::string, Event> mapEvents;
std::unordered_map<std::string, Camera> mapCameras;

// Static object to store data source for current iteration
DataSource currentSource;

// Get events from all URLs
void fetchEvents() {
  //std::cout << "\nFetching NYS Onondaga County 911 events:\n\n";
  //getEvents(ONGOV::EVENTS_URL);
  //std::cout << "\nFetching NYS 511 events:\n\n";
  //getEvents(NYSDOT::EVENTS_URL);
  //std::cout << "\nFetching Monroe County 911 events:\n\n";
  //getEvents(MCNY::EVENTS_URL);
  //std::cout << "\nFetching Ontario 511 events:\n\n";
  //getEvents(ONMT::EVENTS_URL);
  std::cout << "\nFetching Ottawa events:\n\n";
  getEvents(OTT::EVENTS_URL);
  
  // TODO:
  //std::cout << "\nFetching Montreal events:\n\n";
  //getEvents(MTL::EVENTS_URL);
}

void fetchCameras() {
  std::cout << "\nFetching NYS 511 cameras:\n\n";
  getCameras(NYSDOT::CAMERAS_URL);
}

// Print all events in the map
void printEvents() {
  for(const auto& [key, event] : mapEvents) {
    std::cout << event;
  }
}

// Retrieve all events from the URL
bool getEvents(std::string url) {
  // Check for Data Source
  if(url.find("511ny.org") != std::string::npos) {
    // Source API key
    if(NYSDOT::API_KEY.empty()) {
      NYSDOT::getEnv();
      if(NYSDOT::API_KEY.empty()) {
        std::cerr << Output::Colors::RED << "[Traffic] ERROR: Failed to retrieve NYSDOT API Key from local environment.\n" << Output::Colors::END;
        return false;
      }
    }
    url += NYSDOT::API_KEY;
    // Set current Data Source
    currentSource = DataSource::NYSDOT;
  } 
  else if(url.find("511on.ca") != std::string::npos)
    currentSource = DataSource::ONMT;
  else if(url.find("monroecounty.gov") != std::string::npos)
    currentSource = DataSource::MCNY;
  else if(url.find("ongov.net") != std::string::npos)
    currentSource = DataSource::ONGOV;
  else if(url.find("ottawa.ca") != std::string::npos)
    currentSource = DataSource::OTT;
  else {
    currentSource = DataSource::UNKNOWN;
    std::cerr << Output::Colors::RED << "[Traffic] ERROR: Source domain does not match program data requirements.\n" << Output::Colors::END;
    return false;
  }

  // Retrieve data with cURL
  auto [result, data, headers] = cURL::getData(url);

  // Check for successful extraction
  if(result == cURL::Result::SUCCESS) {
    // Make sure response data isnt empty
    if(!data.empty()) {
      processData(data, headers);
    } else {
      // Error out and exit if empty string returned  
      std::cerr << Output::Colors::RED << "[cURL]: Retrieved data string empty.\n" << Output::Colors::END;
      return false;
    }
  } else {
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

// Process retrieved data string and headers
bool processData(std::string& data, const std::vector<std::string>& headers) {
  // Extract the "Content-Type" header
  std::string contentType = cURL::getContentType(headers);
  
  // Check for valid JSON, XML, or HTML response and parse
  if(contentType.find("application/json") != std::string::npos) {
    auto parsedData = JSON::parseData(data);  // Returns a Json::Value object
    parseEvents(parsedData);
  } else if(contentType.find("text/xml") != std::string::npos) {
    auto parsedData = XML::parseData(data);  // Returns a unique_ptr to an xml_document<> into the responseStr
    // Check for parsing success
    if(!parsedData) {
      std::cerr << Output::Colors::RED << "[JSON] Error: parsing failed.\n" << Output::Colors::END;
      return false;
    }
    // Parse the events
    parseEvents(std::move(parsedData)); // Data held by unique_ptr so transfer ownership with std::move
    // NOTE: parsedData has now been invalidated, attmpting to access will result in UB
  } else if(contentType.find("text/html") != std::string::npos) {
    auto parsedData = ONGOV::Gumbo::parseData(data);
    // Check for parsing success
    if(!parsedData) {
      std::cerr << Output::Colors::RED << "[HTML] Error: parsing failed.\n" << Output::Colors::END;
      return false;
    }
    parseEvents(*parsedData);
  } else {
    // Error and exit if invalid type returned
    std::cerr << Output::Colors::RED << "[cURL] ERROR: Unsupported \"Content-Type\": '" <<  contentType << '\n' << Output::Colors::END;
    return false;
  }
  return true;
}

// Parse JSON events
bool parseEvents(const Json::Value& parsedData) {
  // Iterate through each event in the parsed JSON data
  for(const auto& element : parsedData) {
    // Check for valid JSON
    if(element.isObject()) {
      processEvent(element);
    } else if(element.isArray()) {
      processEventArr(element);
    } else {
      std::cerr << Output::Colors::RED << "[JSON] Failed parsing event (is the JSON valid?)\n" << Output::Colors::END;
      continue;
    }
  }
  std::cout << Output::Colors::GREEN << "\n[JSON] Successfully parsed root tree.\n" << Output::Colors::END;
  
  // Clean up cleared events while our data is still in scope
  // TODO: clearEvents needs to work when passed both levels of array
  // Need to iterate through and check for Object/Array type when marking for deletion
  //clearEvents(parsedData);
  return true;
}

// Parse XML events
bool parseEvents(std::unique_ptr<rapidxml::xml_document<>> parsedData) {
  // De-reference the document pointer to access its value
  rapidxml::xml_document<>& events = *parsedData; // NOTE: parsedData is no longer a valid reference

  // Setup the XML tree for processing
  rapidxml::xml_node<>* root = events.first_node("rss"); // Define root entry point
  rapidxml::xml_node<>* channel = root->first_node("channel"); // Navigate to channel
  
  // Iterate throgh each event in the document tree
  for(rapidxml::xml_node<>* event = channel->first_node("item"); event; event = event->next_sibling()) {
    MCNY::processEvent(event); 
  }
  std::cout << Output::Colors::GREEN << "\n[XML] Successfully parsed root tree." << Output::Colors::END << '\n';
  
  // Clean up cleared events while our data is still in scope
  clearEvents(events);  // NOTE: Make sure to pass the dereferenced events data here as parsedData is invalid
  return true;
}

// Parse events from an array of temp HTML events
bool parseEvents(const std::vector<HTML::Event>& parsedData) {
  // Iterate through each parsed event in the vector
  for(const auto& parsedEvent : parsedData) {
    // Try to insert it on the vector
    // Will not add if it already exists
    mapEvents.try_emplace(parsedEvent.ID, parsedEvent);
  }
  std::cout << Output::Colors::GREEN << "\n[HTML] Successfully parsed HTML document." << Output::Colors::END << '\n';
  // Clean up cleared events while our data is still in scope
  clearEvents(parsedData);  // NOTE: Make sure to pass the dereferenced events data here as parsedData is invalid
  return true;
}

// Process a parsed JSON event for storage
bool processEvent(const Json::Value& parsedEvent) {

  // Extract the key and confiorm we have a valid event
  std::string key{""};
  if(parsedEvent.isMember("ID"))
    key = parsedEvent["ID"].asString();
  else if(parsedEvent.isMember("id")) {
    key = parsedEvent["id"].asString();
    std::cout << "Found 'id': " << key << '\n'; 
  }
  else {
    std::cerr << Output::Colors::RED << "[JSON] Error: No 'ID' member present in JSON event.\n" << Output::Colors::END;
    return false;
  }

  // Check if the event is within one of our markets
  if(!inMarket(parsedEvent)) {
    std::cout << "Event not in market!\n";
    return false;
  }

  // Check eventType
  if(!isIncident(parsedEvent)) {
    std::cout << "Event is not a valid type.\n";
    return false;
  }

  // Add the event
  // Try to insert a new Event at event, inserted = false if it already exists
  auto [event, inserted] = mapEvents.try_emplace(key, parsedEvent);
  // Check if we added a new event
  if(!inserted) {
    // Check for updated timestamp
    if(event->second.getLastUpdated() == getTime(parsedEvent)) {
      return false;
    }
    // Update the event
    event->second = parsedEvent;
    std::cout << Output::Colors::MAGENTA << "[JSON] Updated event: " << key << Output::Colors::END << '\n';
  }
  return true;
}

// Process a parsed JSON event Array for storage
bool processEventArr(const Json::Value& parsedArray) {
  for(const auto& event : parsedArray) {
    if(!event.isObject()) {
      std::cerr << Output::Colors::RED << "[JSON] Failed parsing event (is the JSON valid?)\n" << Output::Colors::END;
      continue;
    } else {
      processEvent(event);
    }
  }
  return true;
}

// Check if retrieved JSON data contains an event with given key
bool containsEvent(const Json::Value& events, const std::string& key) {
  for(const auto& event : events) {
    if(!event.isObject() || !event.isMember("ID")) {
      std::cerr << Output::Colors::RED << "[JSON] Parsed event not a valid object!\n" << Output::Colors::END;
      continue;
    }
    // Check for matching key value
    if(event["ID"].asString() == key)
      return true;
    else
      continue;
  }
  return false;
}

// Check if retrieved XML data contains an event with given key
bool containsEvent(rapidxml::xml_document<>& events, const std::string& key) {
  rapidxml::xml_node<>* root = events.first_node("rss"); // Define root entry point
  rapidxml::xml_node<>* channel = root->first_node("channel"); // Navigate to channel

  // Iterate through the XML document
  for(rapidxml::xml_node<>* event = channel->first_node("item"); event; event = event->next_sibling()) {
    // Extract key from description
    std::pair<std::string, std::string> description = MCNY::parseDescription(event->first_node("description"));
    auto& [status, parsedKey] = description;
    // Check for matching key value
    if(parsedKey == key)
      return true;
    else
      continue;
  }
  return false;
}

// Check if parsed HTML events contains an event with the given key
bool containsEvent(const std::vector<HTML::Event>& events, const std::string& key) {
  for(const auto& event : events) {
    if(event.ID == key)
      return true;
  }
  return false;
}

// Check if an event is both in market and of valid type
bool inMarket(const Json::Value& parsedEvent) {
  if(currentSource == DataSource::OTT)
    return true;
  // If we are a NYS event check if we are in region
  if(parsedEvent.isMember("RegionName")) {
    if(!NYSDOT::inRegion(parsedEvent))
      return false;
  } else { // If we are a canadian event check if we are in region 
    // Extract the location
    Location location = getLocation(parsedEvent);
    if(!ONMT::regionToronto.contains(location) && !ONMT::regionOttawa.contains(location))
      return false;
  }
  
  return true;
}

// Get the location from a parsed JSOn event
Location getLocation(const Json::Value& parsedEvent) {
  if(currentSource == DataSource::ONMT)
    return { parsedEvent["Latitude"].asDouble(), parsedEvent["Longitude"].asDouble() };
  else if(currentSource == DataSource::OTT)
    // TODO:
    // Parse location from the JSON and return it here.
    // Location for Ottawa events is located in a geodata:coordinates sub-item as an array/pair
    return { 0, 0 };
  else
      return { 0, 0 };

}

// Check for matching incident type
bool isIncident(const Json::Value& parsedEvent){
  std::string type;
  if(parsedEvent.isMember("EventType"))
    type = parsedEvent["EventType"].asString();
  else if(parsedEvent.isMember("eventType"))
    type = parsedEvent["eventType"].asString();
  return (type == "accidentsAndIncidents"
       || type == "closures"
       || type == "INCIDENT");
}

// Get the last updated time from a parsed event
std::chrono::system_clock::time_point getTime(const Json::Value& parsedEvent){
  // Process Ottawa time
  if(parsedEvent.isMember("updated")) {
    std::string time = parsedEvent["updated"].asString();
    return Time::YYYYMMDDHHMMSS::toChrono(time);
  }

  // Check if we are in unixtime
  if(parsedEvent["LastUpdated"].isInt()){
    int unixtime = parsedEvent["LastUpdated"].asInt();
    return Time::UNIX::toChrono(unixtime, std::nullopt);
  }
  // Else return a NYSDOT time
  std::string time = parsedEvent["LastUpdated"].asString();
  return Time::DDMMYYYYHHMMSS::toChrono(time);
}

// Delete all events that match given keys from the map
// Called by templated "Clean Events" function
void deleteEvents(std::vector<std::string> keys) {
  for(const auto& key : keys) {
    mapEvents.erase(key);
    std::cout << Output::Colors::RED << "[Traffic] Deleted event: " << key << Output::Colors::END << '\n'; 
  }
}

// Constructor objects
// Construct an event from an JSON object
Event::Event(const Json::Value& parsedEvent)
: dataSource{ currentSource }
{
  // Process an Ottawa event
  if(dataSource == DataSource::OTT) {
    // TODO:
    // Create event from OTT here cause
    /* Data Members:
        created
        eventGroupAffected
        eventType
        generation_source
        geodata
        headline
        id
        message
        priority
        schedule
        status
        updated
    */
  } else {
    // Construct shared members for DOT events
    if(parsedEvent.isMember("ID"))
      ID = parsedEvent["ID"].asString();
    if(parsedEvent.isMember("EventType"))
      title = parsedEvent["EventType"].asString();
    if(parsedEvent.isMember("RoadwayName"))
      mainStreet = parsedEvent["RoadwayName"].asString();
    if(parsedEvent.isMember("DirectionOfTravel"))
      direction = parsedEvent["DirectionOfTravel"].asString();
    if(parsedEvent.isMember("Description"))
      description = parsedEvent["Description"].asString();
    if(parsedEvent.isMember("Latitude") && parsedEvent.isMember("Latitude")) {
      location = { parsedEvent["Latitude"].asDouble(), parsedEvent["Longitude"].asDouble() }; 
    }

    // Construct members for current source
    switch(dataSource) {
      // Construct members for NYSDOT
      case DataSource::NYSDOT:
        URL = "https://511ny.org/";
        region = NYSDOT::getRegion(parsedEvent["RegionName"].asString());
        if(region == Region::UNKNOWN)
          std::cerr << Output::Colors::RED << "[JSON Event] Error: Failed to source dataSource member during construction\n"
                    << Output::Colors::END;
        if(parsedEvent.isMember("PrimaryLocation"))
          crossStreet = parsedEvent["PrimaryLocation"].asString();
        if(parsedEvent.isMember("Reported") && parsedEvent.isMember("LastUpdated")) {
          timeReported = Time::DDMMYYYYHHMMSS::toChrono(parsedEvent["Reported"].asString());
          timeUpdated = Time::DDMMYYYYHHMMSS::toChrono(parsedEvent["LastUpdated"].asString());
        }
        break;
      // Construct members for ONMT
      case DataSource::ONMT:
        URL = "https://511on.ca/";
        // Determine the region
        if(ONMT::regionToronto.contains(location))
          region = Region::Toronto;
        if(ONMT::regionOttawa.contains(location))
          region = Region::Ottawa;
        if(parsedEvent.isMember("Reported") && parsedEvent.isMember("LastUpdated")) {
          timeReported = Time::UNIX::toChrono(parsedEvent["Reported"].asDouble(), std::nullopt);
          timeUpdated = Time::UNIX::toChrono(parsedEvent["LastUpdated"].asDouble(), std::nullopt);
        }
        break;
      // Error out in all other cases
      default:
        std::cerr << Output::Colors::RED << "[JSON Event] Error: Tried to construct JSON object from invalid data source\n"
                  << Output::Colors::END;
        break;
      }
  }
  
  std::cout << Output::Colors::YELLOW << "\n[JSON Event] Constructed event: " << ID << "  |  " << region 
            << '\n' << Output::Colors::END << description << '\n';
}

// Construct an event from an XML object
Event::Event(const rapidxml::xml_node<>* item, const std::pair<std::string, std::string> &parsedDescription)
: ID{ parsedDescription.second }, dataSource{ DataSource::MCNY }, region{Region::Rochester}, 
  status{ parsedDescription.first }, timeUpdated{ Time::currentTime() }
{
  if(rapidxml::xml_node<> *url = item->first_node("guid")){
    URL = url->value();
  }
  if(rapidxml::xml_node<> *title = item->first_node("title")){
    description = title->value();
    // Use regex to extract Title, street, direction(optional), and cross(optional)
    auto parsedDescription = MCNY::processTitle(description);
    if(parsedDescription) {
      auto [parsedTitle, parsedStreet, parsedDirection, parsedCross] = *parsedDescription;
      if(parsedDirection)
        direction = *parsedDirection;
      if(parsedCross)
        crossStreet = *parsedCross;
      this->title = parsedTitle;        // Make sure to dereference the "this" member rather than our node*
      mainStreet = parsedStreet;
    }
  }
  if(rapidxml::xml_node<> *pubDate = item->first_node("pubDate")){
    // Try to convert to a time object
    if(auto timeOpt = Time::RFC2822::toChrono(pubDate->value())){
      timeReported = *timeOpt;
    }
  }
  // Parse the location
  rapidxml::xml_node<> *latitude = item->first_node("geo:lat");
  rapidxml::xml_node<> *longitude = item->first_node("geo:long");
  if(latitude && longitude){
    std::string parsedLat = latitude->value();
    std::string parsedLong = longitude->value();
    location = { std::stof(parsedLat.substr(1)), std::stof(parsedLong.substr(1)) * -1 };
  }   
  
  std::cout << Output::Colors::YELLOW << "\n[XML Event] Constructed event: " << ID << "  |  " << region
            << '\n' << Output::Colors::END << description << '\n';
}

// Construct an event from an HTML event
Event::Event(const HTML::Event& parsedEvent)
: ID{ parsedEvent.ID }, URL{ "https://911events.ongov.net/CADInet/app/events.jsp" }, dataSource{ DataSource::ONGOV },
  region{ Region::Syracuse }, location{ Location(43.05, -76.15) }, timeUpdated{ Time::currentTime() }
{
  bool hasMain{ false };    // Flag to check if main address exists
  std::string descStr{ "" };    // Create a string to build and hold the description

  if(parsedEvent.title != "N/A") {
    // Process the event title
    title = parsedEvent.title;
    // Add the title to the string followed by " at "
    descStr += parsedEvent.title + " at ";
  }
  if(parsedEvent.address != "N/A") {
    // Process the main street
    auto parsedAddress = ONGOV::processAddress(parsedEvent.address);
    if(parsedAddress) {
      auto [parsedStreet, parsedDir] = *parsedAddress;
      if(parsedDir)
        direction = *parsedDir;
      mainStreet = parsedStreet;
    }
    // Add the address to the string followed by
    descStr += parsedEvent.address + ' ';
    hasMain = true;
  }
  if(parsedEvent.xstreet != "N/A") {
    if(!hasMain) {
      // Process cross street as main
      auto parsedCross = ONGOV::processCrossAsAddress(parsedEvent.xstreet); // Returns an optional pair of an addressDir and a crossStreet
      if(parsedCross) {
        auto [mainRoadDir, crossRoad] = *parsedCross;

        mainStreet = mainRoadDir.first;
        std::optional<std::string>dir = mainRoadDir.second;

        if(dir) {
          direction = *dir;        }

        if(crossRoad)
          crossStreet = *crossRoad;
      }
    } else {
      // Process cross street as cross
      crossStreet = parsedEvent.xstreet;
    }
    // Add the cross street to the string
    descStr += "( X: " + parsedEvent.xstreet + " ) ";
  }
  if(parsedEvent.date != "N/A") {
    // Process the event date
    timeReported = Time::MMDDYYHHMM::toChrono(parsedEvent.date);
    // Add the date to the string
    descStr += "[ " + parsedEvent.date + " ]";
  }

  description = std::move(descStr);
  
  std::cout << Output::Colors::YELLOW << "\n[HTML Event] Constructed event: " << ID
            << '\n' << Output::Colors::END << description << '\n';
}

// Move constructor for an event object
Event::Event(Event&& other) noexcept 
: ID(std::move(other.ID)),
  URL(std::move(other.URL)),
  dataSource(other.dataSource),
  region(other.region),
  title(std::move(other.title)),
  status(std::move(other.status)),
  mainStreet(std::move(other.mainStreet)),
  crossStreet(std::move(other.crossStreet)),
  direction(std::move(other.direction)),
  description(std::move(other.description)),
  location(other.location),
  timeReported(std::move(other.timeReported)),
  timeUpdated(std::move(other.timeUpdated))
{
  std::cout << Output::Colors::BLUE << "[Event] Moved event: " << ID  << "  |  " << region
            << '\n' << Output::Colors::END << description << '\n';

}

// Move assignment operator for event object
Event& Event::operator=(Event&& other) noexcept {
  // Check for self-assignment
  if(this != &other) {
    ID = std::move(other.ID);
    URL = std::move(other.URL);
    dataSource = other.dataSource;
    region = other.region;
    title = std::move(other.title);
    status = std::move(other.status);
    mainStreet = std::move(other.mainStreet);
    crossStreet = std::move(other.crossStreet);
    direction = std::move(other.direction);
    description = std::move(other.description);
    location = other.location;
    timeReported = std::move(other.timeReported);
    timeUpdated = std::move(other.timeUpdated);
  }
  std::cout << Output::Colors::BLUE << "[Event] Invoked move assignment: " << ID << "  |  " << region
            << '\n' << Output::Colors::END << description << '\n';
  return *this;
}


// TODO:
// Camera functions
bool getCameras(std::string url){
  // Source API key
  if(NYSDOT::API_KEY.empty())
    NYSDOT::getEnv();
  assert(!NYSDOT::API_KEY.empty() && "Failed to retrieve API key from local environment.");
  url += NYSDOT::API_KEY;
  // Set current Data Source
  currentSource = DataSource::NYSDOT;
  
  // Retrieve data with cURL
  auto [result, data, headers] = cURL::getData(url);
  
  // Check for successful extraction
  if(result == cURL::Result::SUCCESS) {
    // Make sure response data isnt empty
    if(!data.empty()) {
      //processData(data, headers);
      parseCameras(data);
    } else {
      // Error out and exit if empty string returned  
      std::cerr << Output::Colors::RED << "[cURL]: Retrieved data string empty.\n" << Output::Colors::END;
      return false;
    }
  } else {
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

bool parseCameras(const std::string& data) {
  auto parsedData = JSON::parseData(data);
  for(const auto& camera : parsedData) {
    if(!camera.isObject()) {
      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing camera (is the JSON valid?)\n" << Output::Colors::END;
      return false; // Or do we want to continue here?
    } else
      processCamera(camera);
  }
  std::cout << Output::Colors::GREEN << "\n[JSON] Successfully parsed root tree.\n" << Output::Colors::END;
  return true;
}

bool processCamera(const Json::Value& parsedCamera) {
  if(!parsedCamera.isMember("ID")) {
    std::cerr << Output::Colors::RED << "[JSON] Error: No 'ID' member present in JSON event.\n" << Output::Colors::END;
    return false;
  }

  // Extract key/ID from the event
  std::string key = parsedCamera["ID"].asString();
  Location location = { parsedCamera["Latitude"].asDouble(), parsedCamera["Longitude"].asDouble() };

  if(!NYSDOT::regionSyracuse.contains(location)) // TODO: Add more regions
    return false;

  auto [camera, inserted] = mapCameras.try_emplace(key, parsedCamera);
  if(!inserted) {
    if(parsedCamera["Disabled"].asBool() == camera->second.isOnline()) {
      return false;
    }
    camera->second = parsedCamera;
    std::cout << Output::Colors::MAGENTA << "[JSON] Updated event: " << key << Output::Colors::END << '\n';
  }
  return true;
}

// Camera constructors
Camera::Camera(const Json::Value& parsedCamera)
: dataSource{ currentSource }
{
  if(parsedCamera.isMember("ID"))
    ID = parsedCamera["ID"].asString();
  if(parsedCamera.isMember("Name"))
    description = parsedCamera["Name"].asString();
  if(parsedCamera.isMember("Url"))
    imageURL = parsedCamera["Url"].asString();
  if(parsedCamera.isMember("VideoUrl"))
    videoURL = parsedCamera["VideoUrl"].asString();
  if(parsedCamera.isMember("Disabled") || parsedCamera.isMember("Blocked")) {
    if(!parsedCamera["Disabled"].asBool() && !parsedCamera["Blocked"].asBool())
      online = true;
  }
  if(parsedCamera.isMember("RoadwayName"))
    roadwayName = parsedCamera["RoadwayName"].asString();
  if(parsedCamera.isMember("DirectionOfTravel"))
    direction = parsedCamera["DirectionOfTravel"].asString();
  if(parsedCamera.isMember("Latitude") && parsedCamera.isMember("Latitude"))
    location = { parsedCamera["Latitude"].asDouble(), parsedCamera["Longitude"].asDouble() }; 
  
  std::cout << Output::Colors::YELLOW << "\n[JSON Camera] Constructed camera: " << ID << "  |  " << region 
            << '\n' << Output::Colors::END << description << '\n';
}

Camera::Camera(Camera&& other) noexcept
: ID(std::move(other.ID)),
  dataSource(other.dataSource),
  description(std::move(other.description)),
  imageURL(std::move(other.imageURL)),
  videoURL(std::move(other.videoURL)),
  online(other.online),
  region(other.region),
  roadwayName(std::move(other.roadwayName)),
  direction(std::move(other.direction)),
  location(other.location)
{
  std::cout << Output::Colors::BLUE << "[Camera] Moved camera: " << ID  << "  |  " << region
            << '\n' << Output::Colors::END << description << '\n';
}

Camera& Camera::operator=(Camera&& other) noexcept
{
  if(this != &other) {
    ID = std::move(other.ID);
    dataSource = other.dataSource;
    description = std::move(other.description);
    imageURL = std::move(other.imageURL);
    videoURL = std::move(other.videoURL);
    online = other.online;
    region = other.region;
    roadwayName = std::move(other.roadwayName);
    direction = std::move(other.direction);
    location = other.location;
  }
  std::cout << Output::Colors::BLUE << "[Camera] Invoked move assignment: " << ID << "  |  " << region
            << '\n' << Output::Colors::END << description << '\n';
  return *this;
}

// Output Operators

std::ostream& operator<<(std::ostream& out, const Region& region) {
  switch(region) {
    case Region::Syracuse:
      out << "Syracuse, NY";
      break;
    case Region::Rochester:
      out << "Rochester, NY";
      break;
    case Region::Buffalo:
      out << "Buffalo, NY";
      break;
    case Region::Albany:
      out << "Albany, NY";
      break;
    case Region::Binghamton:
      out << "Binghamton, NY";
      break;
    case Region::Toronto:
      out << "Toronto, ON";
      break;
    case Region::Ottawa:
      out << "Ottawa, ON";
      break;
    case Region::Montreal:
      out << "Montreal, QC";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream& operator<<(std::ostream& out, const DataSource& dataSource) {
  switch(dataSource) {
    case DataSource::NYSDOT:
      out << "NYS 511";
      break;
    case DataSource::ONGOV:
      out << "Onondaga Counmty 911";
      break;
    case DataSource::MCNY:
      out << "Monroe County 911";
      break;
    case DataSource::ONMT:
      out << "Ontario 511";
      break;
    case DataSource::OTT:
      out << "Ottawa City";
      break;
    case DataSource::MTL:
      out << "Quebec 511";
      break;
    default:
      out << "Unknown";
      break;
  }
  return out;
}

std::ostream &operator<<(std::ostream &out, const Event &event){
  std::tm timeReported = Time::toLocalPrint(event.timeReported);
  std::tm timeUpdated = Time::toLocalPrint(event.timeUpdated);

  out << '\n' << event.region << " (" << event.dataSource << ")  |  " << event.ID << "  |  " << event.status << '\n'
      << event.title << '\n'
      << event.mainStreet << "(" << event.direction << ") at " << event.crossStreet << "  |  " << event.location << '\n'
      << event.description << '\n'
      << event.URL << '\n'
      << "Reported: " << std::put_time(&timeReported, "%T - %F") << "  |  Updated: " << std::put_time(&timeUpdated, "%T - %F")
      << std::endl;
  return out;
}

} // namespace Traffic
