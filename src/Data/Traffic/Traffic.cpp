#include "Traffic.h"
#include "NYSDOT.h"
#include "MCNY.h"
#include "ONMT.h"
#include "MTL.h"
#include "OTT.h"
#include "ONGOV.h"
#include "DataUtils.h"
#include "Output.h"
#include "RestAPI.h"
#include "rapidxml.hpp"
#include <chrono>
#include <json/value.h>
#include <optional>
#include <mutex>    
#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <unordered_map>

/* 
 TODO:
 *
 * DATA COLLECTION & PROCESSING
 *
 *    ONGOV:
 *      Investigate REST API
 *        Possible workarounds for geo-restriction
 *          Do we need to proxy requests locally on client?
 *        Add logic to account for multiple pages
 *          Probably need to modify to POST request and investigate payloads in-browser
 *          Do we need to establish a session and maintain state?
 *      Modify cleanup to only run if we get valid table data
 *      Create an async function which can be run every few minutes to set geo-coordinates
 *        If we use openstreetmap we are limited to one request per second.
 *        Setup an atomic timer!
 *
 *    ONMT: 
 *      Normalize data for Ontario events
 *        Need to extract optional side road if it exists, rest seems to be parsing fine
 *        Check reported time against current time to filter out future (planned) events
 *
 *    OTT:
 *      Fix parsing against more test cases
 *        Need to gather more data to test against!
 *        Should we parse against the headline or the full description?
 *
 *    MTL: 
 *      Use REGEX to parse description
 *      Redefine global cleanup to work with Montreal events
 *      Create an async function which can be run every few minutes to set geo-coordinates
 *        If we use openstreetmap we are limited to one request per second.
 *        Setup an atomic timer!
 * 
 * DATA INTERFACE
 *    
 *    Serializing to JSON:
 *      We should serialize time to an ISO6801-formatted string
 *    Serve serialized JSON via RESTful endpoints
 *    Implement robust filtering via query params
 */

namespace Traffic {

// Data structures
std::mutex eventsMutex;
std::unordered_map<std::string, Event> mapEvents;
std::unordered_map<std::string, Camera> mapCameras;

// Static object to store data source for current iteration
DataSource currentSource;

Region toRegion(const std::string& regionStr) {
  if(regionStr == "Syracuse" || regionStr == "syracuse")
    return Region::Syracuse;
  if(regionStr == "Rochester" || regionStr == "rochester")
    return Region::Rochester;
  if(regionStr == "Buffalo" || regionStr == "buffalo")
    return Region::Buffalo;
  if(regionStr == "Albany" || regionStr == "albany")
    return Region::Albany;
  if(regionStr == "Binghamton" || regionStr == "binghamton")
    return Region::Binghamton;
  if(regionStr == "Toronto" || regionStr == "toronto")
    return Region::Toronto;
  if(regionStr == "Ottawa" || regionStr == "ottawa")
    return Region::Ottawa;
  if(regionStr == "Montreal" || regionStr == "montreal")
    return Region::Montreal;

  return Region::UNKNOWN;
}

DataSource toSource(const std::string& sourceStr) {
  if(sourceStr == "NYSDOT" || sourceStr == "nysdot" || sourceStr == "511ny" || sourceStr == "511NY")
    return DataSource::NYSDOT;
  if(sourceStr == "ONGOV" || sourceStr == "ongov" || sourceStr == "Onondaga911" || sourceStr == "onondaga911")
    return DataSource::ONGOV;
  if(sourceStr == "MCNY" || sourceStr == "mcny" || sourceStr == "Monroe911" || sourceStr == "monroe911")
    return DataSource::MCNY;
  if(sourceStr == "ONMT" || sourceStr == "onmt" || sourceStr == "511on" || sourceStr == "511ON")
    return DataSource::ONMT;
  if(sourceStr == "Ottawa" || sourceStr == "ottawa" || sourceStr == "OTT" || sourceStr == "ott")
    return DataSource::OTT;
  if(sourceStr == "MTL" || sourceStr == "mtl" || sourceStr == "Montreal" || sourceStr == "montreal")
    return DataSource::MTL;

  return DataSource::UNKNOWN;
}

// Get events from all URLs
void fetchEvents() {
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching Onondaga County 911 events");
  getEvents(ONGOV::EVENTS_URL);
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching NYS 511 events");
  getEvents(NYSDOT::EVENTS_URL);
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching Monroe County 911 events");
  getEvents(MCNY::EVENTS_URL);
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching Ontario 511 events");
  getEvents(ONMT::EVENTS_URL);
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching Ottawa events");
  getEvents(OTT::EVENTS_URL);
  Output::logger.log(Output::LogLevel::INFO, "EVENTS", "Fetching Montréal events");
  getEvents(MTL::EVENTS_URL);
}

void fetchCameras() {
  Output::logger.log(Output::LogLevel::INFO, "CAMERAS", "Fetching NYS 511 cameras");
  getCameras(NYSDOT::CAMERAS_URL);
}

// Print all events in the map
void printEvents() {
  // Lock the map for reading
  std::lock_guard<std::mutex> lock(eventsMutex);
  for(auto& [key, event] : mapEvents) {
    event.print();
  }
}

void printEvents(Region region) {
  // Lock the map for reading
  std::lock_guard<std::mutex> lock(eventsMutex);
  for(auto& [key, event] : mapEvents) {
    if(event.getRegion() == region) {
      event.print();
    }
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
        Output::logger.log(Output::LogLevel::ERROR, "ENV", "Failed to retrieve NYSDOT API Key from local environment");
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
  else if(url.find("quebec511.info") != std::string::npos)
    currentSource = DataSource::MTL;
  else {
    currentSource = DataSource::UNKNOWN;
    Output::logger.log(Output::LogLevel::WARN, "EVENTS", "Source domain does not match program data requirements");
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
      Output::logger.log(Output::LogLevel::WARN, "cURL", "Retrieved empty data string");
      return false;
    }
  } else {
    switch(result) {
      case cURL::Result::TIMEOUT:
        Output::logger.log(Output::LogLevel::WARN, "cURL", "Timed out retrieving data from remote stream. Retrying in 60 seconds");
        break;
      default:
        Output::logger.log(Output::LogLevel::ERROR, "cURL", "Critical error retrieiving data from remote stream. Terminating program");
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
    // Convert encoding for MTL data
    if(currentSource == DataSource::MTL)
      data = convertEncoding(data, "ISO-8859-1", "UTF-8");
    auto parsedData = XML::parseData(data);  // Returns a unique_ptr to an xml_document<> into the responseStr
    // Check for parsing success
    if(!parsedData) {
      Output::logger.log(Output::LogLevel::WARN, "JSON", "Failed to parse data stream");
      return false;
    }
    // Parse the events
    parseEvents(std::move(parsedData)); //  NOTE: parsedData has now been invalidated, attmpting to access will result in UB
  } else if(contentType.find("text/html") != std::string::npos) {
    auto parsedData = ONGOV::Gumbo::parseData(data);
    // Check for parsing success
    if(!parsedData) {
      Output::logger.log(Output::LogLevel::WARN, "HTML", "Failed to parse data stream");
      return false;
    }
    parseEvents(*parsedData);
  } else {
    // Error and exit if invalid type returned
    std::string errMsg = "Unsupported \"Content-Type\": '" + contentType + "'";
    Output::logger.log(Output::LogLevel::WARN, "cURL", errMsg);
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
      parseEvents(element); // Recursively parse through arrays
    } else {
      Output::logger.log(Output::LogLevel::WARN, "JSON", "Failed parsing event (is the JSON valid?)");
      continue;
    }
  }
  
  // Clean up cleared events while our data is still in scope
  if(parsedData.isArray() && !parsedData.empty() && parsedData[0].isObject())   // Since we are recursive, need to only delete if we are at the event array
    clearEvents(parsedData);
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
    // Lock the map before processing the event
    std::lock_guard<std::mutex> lock(eventsMutex);
    if(currentSource == DataSource::MCNY)
      MCNY::processEvent(event); 
    else if(currentSource == DataSource::MTL)
      MTL::processEvent(event);
  }
  
  // Clean up cleared events while our data is still in scope
  clearEvents(events);  // NOTE: Make sure to pass the dereferenced events data here as parsedData is invalid
  
  return true;
}

// Parse events from an array of temp HTML events
bool parseEvents(const std::vector<HTML::Event>& parsedData) {
  // Iterate through each parsed event in the vector
  for(const auto& parsedEvent : parsedData) {
    // Lock the maop here
    std::lock_guard<std::mutex> lock(eventsMutex);
    // Try to insert it on the vector
    // Will not add if it already exists
    mapEvents.try_emplace(parsedEvent.ID, parsedEvent);
  }
  // Clean up cleared events while our data is still in scope
  clearEvents(parsedData);
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
  }
  else {
    return false;
  }

  // Check if the event is within one of our markets
  if(!inMarket(parsedEvent)) {
    return false;
  }

  // Check eventType
  if(!isIncident(parsedEvent)) {
    return false;
  }

  // Lock the map before inserting
  std::lock_guard<std::mutex> lock(eventsMutex);

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
    std::string msg = "Updated event: " + key;
    Output::logger.log(Output::LogLevel::INFO, "JSON", msg);
  }
  return true;
}

// Check if retrieved JSON data contains an event with given key
bool containsEvent(const Json::Value& events, const std::string& key) {
  for(const auto& event : events) {
    if(!event.isObject() || !(event.isMember("ID") || event.isMember("id"))) {
      Output::logger.log(Output::LogLevel::WARN, "JSON", "Parsed event is not a valid object");
      continue;
    }

    std::string id;
    // Check for matching key value
    if(event.isMember("ID"))
      id = event["ID"].asString();
    else if(event.isMember("id"))
      id = event["id"].asString();
    
    if(id == key)
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
  // Process for MCNY
  if(currentSource == DataSource::MCNY) {
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
    // Process for MTL
  } else if(currentSource == DataSource::MTL) {
    for(rapidxml::xml_node<>* event = channel->first_node("item"); event; event = event->next_sibling()) {
      // Extract the url
      std::string url;
      if(rapidxml::xml_node<>* link = event->first_node("link")){
        url = link->value();
      }
      // And the ID
      std::string id = MTL::extractID(url);
      if(id.empty()) {
        continue;
      }
      // Check for matching key value
      if(id == key)
        return true;
      else
        continue;
    }
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
  assert(currentSource != DataSource::OTT); // We should never call this if we are in OTT, no need to check
  return { parsedEvent["Latitude"].asDouble(), parsedEvent["Longitude"].asDouble() };
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
// NOTE: Locking already implemented within clearEvents, our map is already locked at this point
void deleteEvents(std::vector<std::string> keys) {
  for(const auto& key : keys) {
    mapEvents.erase(key);
    std::string msg = "Deleted event: " + key;
    Output::logger.log(Output::LogLevel::INFO, "EVENTS", msg);
  }
}

// Serialize all traffic events into Json objects
std::optional<Json::Value> serializeEventsToJSON(const std::vector<std::pair<std::string, std::string>>& queryParams) {
  // Create an array to hold all events
  Json::Value eventsArray(Json::arrayValue);
  // Create optional filter values
  std::optional<Region> filterRegion{std::nullopt};
  std::optional<DataSource> filterSource{std::nullopt};

  // Extract filter parameters
  auto regionParam = RestAPI::findQueryParam(queryParams, "region");
  auto sourceParam = RestAPI::findQueryParam(queryParams, "source");
  // Error out if we have invalid keys
  if(!(regionParam || sourceParam || queryParams.empty()))
    return std::nullopt;
  if(regionParam) {
    // Set the filter value
    filterRegion = toRegion(*regionParam);
  } 
  if(sourceParam) {
    // Set the filter value
    filterSource = toSource(*sourceParam);
  }

  
  // Serialize the data
  // Lock the map to this thread for reading
  std::lock_guard<std::mutex> lock(eventsMutex);    // Must be locked before entering the loop to prevent iterator invalidation
  // And read the map
  for(const auto& [key, event] : mapEvents) {
    // Check if we have a region filter
    if((filterRegion && event.getRegion() != *filterRegion) || (filterSource && event.getSource() != *filterSource))
      continue;
    
    // Else we either aren't filtering, or filter matches, so we want to add
    // Create a json item from the event
    Json::Value item;
    event.serializeToJSON(item);

    // Add the item to the root object
    eventsArray.append(item);
  }

  return eventsArray;
}


void Event::serializeToJSON(Json::Value& item) const {
  // Store fields in JSON
  // String fields
  item["id"] = ID;

  if(URL == "N/A")
    item["url"] = Json::nullValue;
  else
    item["url"] = URL;

  if(title == "N/A")
    item["title"] = Json::nullValue;
  else
    item["title"] = title;

  item["status"] = status;

  if(mainStreet == "N/A")
    item["main"] = Json::nullValue;
  else
    item["main"] = mainStreet;

  if(crossStreet == "N/A")
    item["secondary"] = Json::nullValue;
  else
    item["secondary"] = crossStreet;

  if(direction == "N/A")
    item["direction"] = Json::nullValue;
  else
    item["direction"] = direction;

  if(description == "N/A")
    item["description"] = Json::nullValue;
  else
    item["description"] = description;

  switch(dataSource) {
    case DataSource::NYSDOT:
      item["source"] = "NYSDOT";
      break;
    case DataSource::ONGOV:
      item["source"] = "ONGOV";
      break;
    case DataSource::MCNY:
      item["source"] = "MCNY";
      break;
    case DataSource::ONMT:
      item["source"] = "ONMT";
      break;
    case DataSource::OTT:
      item["source"] = "OTT";
      break;
    case DataSource::MTL:
      item["source"] = "MTL";
      break;
    default:
      item["source"] = Json::nullValue;
      break;
  }

  switch(region) {
    case Region::Syracuse:
      item["region"] = "Syracuse";
      break;
    case Region::Rochester:
      item["region"] = "Rochester";
      break;
    case Region::Buffalo:
      item["region"] = "Buffalo";
      break;
    case Region::Albany:
      item["region"] = "Albany";
      break;
    case Region::Binghamton:
      item["region"] = "Binghamton";
      break;
    case Region::Toronto:
      item["region"] = "Toronto";
      break;
    case Region::Ottawa:
      item["region"] = "Ottawa";
      break;
    case Region::Montreal:
      item["region"] = "Montreal";
      break;
    default:
      item["region"] = Json::nullValue;
      break;
  }
  
  // Create a Json object to hold the latitude and longitude
  Json::Value coordinates;
  if(getCoordinates() == std::make_pair(0.0, 0.0)) {
    coordinates["lat"] = Json::nullValue;
    coordinates["long"] = Json::nullValue;
  } else {
    coordinates["lat"] = getLocation().latitude;
    coordinates["long"] = getLocation().longitude;
  }
  item["coordinates"] = coordinates;

  if(timeReported != std::chrono::system_clock::time_point{}) 
    item["reported"] = Time::ISO6801::toString(timeReported);
  else
    item["reported"] = Json::nullValue;

  if(timeUpdated != std::chrono::system_clock::time_point{}) 
    item["updated"] = Time::ISO6801::toString(timeUpdated);
  else
    item["updated"] = Json::nullValue;
}


// Constructor objects
// Construct an event from an JSON object
Event::Event(const Json::Value& parsedEvent)
: dataSource{ currentSource }
{
  // Process an Ottawa event
  if(dataSource == DataSource::OTT) {
    URL = "https://traffic.ottawa.ca/en/traffic-map-data-lists-and-resources/incidents-construction-and-special-events";
    region = Region::Ottawa;
    if(parsedEvent.isMember("id"))
      ID = parsedEvent["id"].asString();
    if(parsedEvent.isMember("eventType"))
      title = parsedEvent["eventType"].asString();
    if(parsedEvent.isMember("status"))
      status = parsedEvent["status"].asString();
    if(parsedEvent.isMember("message"))
      description = parsedEvent["message"].asString();

    // Extract the location
    if(parsedEvent.isMember("geodata")) {
      if(parsedEvent["geodata"].isObject()) {
        auto geodata = parsedEvent["geodata"];
        if(geodata.isMember("coordinates")) {
          if(geodata["coordinates"].isString()) {
            auto parsedCoordinates = OTT::parseLocation(geodata["coordinates"].asString());
            if(parsedCoordinates)
              location = *parsedCoordinates;
          }
        }
      }
    }

    if(parsedEvent.isMember("created"))
      timeReported = Time::YYYYMMDDHHMMSS::toChrono(parsedEvent["created"].asString());
    if(parsedEvent.isMember("updated"))
      timeUpdated = Time::YYYYMMDDHHMMSS::toChrono(parsedEvent["updated"].asString());

// NOTE:
//  Confirm correct regex parsing of headline
//  We need more test cases

    if(parsedEvent.isMember("headline")) {
      auto parsedHeadline = OTT::parseHeadline(parsedEvent["headline"].asString());
      if(parsedHeadline) {
        auto [road, dir, cross] = *parsedHeadline;
        mainStreet = road;
        if(dir)
          direction = *dir;
        if(cross)
          crossStreet = *cross;
      }
    }
    
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
          Output::logger.log(Output::LogLevel::WARN, "JSON", "Failed to parse dataSource member during construction");
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
        Output::logger.log(Output::LogLevel::WARN, "JSON", "Tried to construct JSON object from invalid data source");
        break;
      }
  }
}

// Construct an event from a Rochester XML object
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
}

// Construct an event from a Montreal XML object
Event::Event(const rapidxml::xml_node<>* parsedEvent) 
: dataSource{ DataSource::MTL }, region{ Region::Montreal }, location{ Location(45.5019, -73.5674) }, timeUpdated{ Time::currentTime() }
{
  // Set the ID and URL
  std::string url;
  if(rapidxml::xml_node<>* link = parsedEvent->first_node("link")){
    url = link->value();
  }
  std::string id;
  if(!url.empty()) {
    id = MTL::extractID(url);
    URL = url;
  }
  if(!id.empty())
    ID = id;

  // Set the Title and main street
  if(rapidxml::xml_node<>* title = parsedEvent->first_node("title")){
    auto parsedTitle = MTL::parseTitle(title->value());
    if(parsedTitle) {
      auto [roadway, eventType] = *parsedTitle;
      mainStreet = roadway;
      if(eventType)
        this->title = *eventType;
    }
  }

  // Set the description
  // Description is stored within a CDATA element
  if(rapidxml::xml_node<>* description = parsedEvent->first_node("description")){
    std::string details = description->value();
    // Check if we extracted a data string or need to parse further for CDATA
    if(details.empty()) {
      // Parse the CDATA node
      rapidxml::xml_node<>* cdataNode = description->first_node();
      if(cdataNode && (cdataNode->type() == rapidxml::node_cdata)) {
        details = cdataNode->value();
      }
    }
    if(!details.empty())
      this->description = details;
    // TODO:
    // Parse description into several elements
    // Or do we want to just sanitize newline chars and store as descritpion
    // Only use values parsed from the title instead (much easier)
    //
    // Elements are delimited via new line so should be relatively simple
    //auto parsedDescription = parseDescription(details);
    /*
     * Line 1       Town name
     * Line 2       Main Roadway
     * Line 3       Between [CROSS] and [CROSS]
     * Line 4       Details (lanes/closure)
     * Line 5
     *
     *
     * */
    
    // Set the cross street
    // Set the direction
  }

  //Set the timereported
  if(rapidxml::xml_node<>* pubDate = parsedEvent->first_node("pubDate")){
    auto timeOpt = Time::RFC2822::toChrono(pubDate->value());
    if(timeOpt)
      timeReported = *timeOpt;
  }

}

// Construct an event from an HTML event
Event::Event(const HTML::Event& parsedEvent)
: ID{ parsedEvent.ID }, URL{ "https://911events.ongov.net/CADInet/app/events.jsp" }, dataSource{ DataSource::ONGOV },
  region{ Region::Syracuse }, location{ Location(43.0495, -76.1474) }, timeUpdated{ Time::currentTime() }
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
    // Prevent crashes! check for strlength
    if(parsedEvent.date.length() == 14) {
      // Process the event date
      timeReported = Time::MMDDYYHHMM::toChrono(parsedEvent.date);
    }
    // Add the date to the string
    descStr += "[ " + parsedEvent.date + " ]";
  }

  description = std::move(descStr);
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
  return *this;
}

//
void Event::print() {
  std::cout << *this;
  printed = true;
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
//      std::cerr << Output::Colors::RED << "[cURL]: Retrieved data string empty.\n" << Output::Colors::END;
      return false;
    }
  } else {
    switch(result) {
      case cURL::Result::TIMEOUT:
//        std::cerr << Output::Colors::RED << "[cURL] Timed out retrieving data from remote stream. Retrying in 60 seconds..." << Output::Colors::END;
        break;
      default:
//        std::cerr << Output::Colors::RED << "[cURL] Critical error retrieiving data from remote stream. Terminating program." << Output::Colors::END;
        return false;
    }
  }

  return true;
}

bool parseCameras(const std::string& data) {
  auto parsedData = JSON::parseData(data);
  for(const auto& camera : parsedData) {
    if(!camera.isObject()) {
//      std::cerr << Output::Colors::RED << "[NYSDOT] Failed parsing camera (is the JSON valid?)\n" << Output::Colors::END;
      return false; // Or do we want to continue here?
    } else
      processCamera(camera);
  }
//  std::cout << Output::Colors::GREEN << "\n[JSON] Successfully parsed root tree.\n" << Output::Colors::END;
  return true;
}

bool processCamera(const Json::Value& parsedCamera) {
  if(!parsedCamera.isMember("ID")) {
//    std::cerr << Output::Colors::RED << "[JSON] Error: No 'ID' member present in JSON event.\n" << Output::Colors::END;
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
//    std::cout << Output::Colors::MAGENTA << "[JSON] Updated event: " << key << Output::Colors::END << '\n';
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
      out << "Onondaga County 911";
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

  out << '\n' << Output::Colors::CYAN << event.region << " (" << event.dataSource << ")  |  " << event.ID << "  |  " << event.status << Output::Colors::END << '\n'
      << event.title << '\n'
      << event.mainStreet << "(" << event.direction << ") at " << event.crossStreet << "  |  " << event.location << '\n'
      << event.description << '\n'
      << event.URL << '\n'
      << "Reported: " << std::put_time(&timeReported, "%T - %F") << "  |  Updated: " << std::put_time(&timeUpdated, "%T - %F")
      << std::endl;
  return out;
}

} // namespace Traffic
