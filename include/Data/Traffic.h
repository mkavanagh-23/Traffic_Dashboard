#ifndef TRAFFIC_H
#define TRAFFIC_H

// TODO:
// Use std::chrono or ctime to normalize to a common time type
// std::optional??, or at least think about how to handle null values and/or default types
#include "DataUtils.h"
#include "Output.h"
#include <iostream>
#include <json/json.h>
#include <memory>
#include <rapidxml.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>

namespace Traffic {

enum class Region {
  Syracuse,
  Rochester,
  Buffalo,
  Albany,
  Binghamton,
  Toronto,
  Ottawa,
  UNKNOWN
};
std::ostream& operator<<(std::ostream& out, const Region& region);

enum class DataSource {
  NYSDOT,
  ONMT,
  MCNY,
  UNKNOWN
};
std::ostream& operator<<(std::ostream& os, const DataSource& dataSource);

extern DataSource currentSource;

class Camera {
private:
  std::string ID;
  DataSource dataSource{ DataSource::NYSDOT };
  std::string description;
  std::string imageURL;
  std::string videoURL;
  bool online{ false };
  Region region{ Region::Syracuse };
  std::string roadwayName;
  std::string direction{ "Unknown" };
  Location location;

public:
  // Constructors
  Camera(const Json::Value& parsedCamera);
  Camera(Camera&& other) noexcept;
  Camera& operator=(Camera&& other) noexcept;

  // Accessors
  std::string_view getID() const { return ID; }
  DataSource getSource() const { return dataSource; }
  std::string_view getDescription() const { return description; }
  bool isOnline() const { return online; }
  Region getRegion() const { return region; }
  Location getLocation() const { return location; }
  std::string_view getStream() const { return videoURL; }
  std::string_view getImage() const { return imageURL; }
};

// Get cameras from all sources
bool getCameras(std::string url);
bool parseCameras(const std::string& data);
bool processCamera(const Json::Value& parsedCamera);

class Event {
private:
  std::string ID;
  DataSource dataSource;
  std::string status{ "Active" };
  Region region;
  std::string roadwayName{ "Unknown" };
  std::string directionOfTravel{ "None" };
  std::string description;  // MCNY 'title' field
  std::chrono::system_clock::time_point timeReported;
  std::chrono::system_clock::time_point timeUpdated;
  Location location;
  //std::vector<Camera> cameras;

public:
  // Constructors
  Event(const Json::Value& parsedEvent);
  Event(const rapidxml::xml_node<>* item, const std::pair<std::string, std::string> &description);
  Event(Event&& other) noexcept;
  Event& operator=(Event&& other) noexcept;

  // Operators
  friend std::ostream &operator<<(std::ostream &out, const Event &event);

  // Accessors
  std::string_view getID() const { return ID; }
  DataSource getSource() const { return dataSource; }
  std::string_view getStatus() const { return status; }
  std::chrono::system_clock::time_point getLastUpdated() const { return timeUpdated; }
  Region getRegion() const { return region; }
  Location getLocation() const { return location; }
  std::string_view getDescription() const { return description; }
  //const std::vector<Camera>& getCameras() const { return cameras; } // Return by const reference, ensure no dangling references!!!
}; // class Event

// Define extern event data structures
extern std::unordered_map<std::string, Event> mapEvents;

// Get events from all sources
void fetchEvents();
void fetchCameras();
void printEvents();
bool getEvents(std::string url);
bool processData(std::string& data, const std::vector<std::string>& headers);   // XML must be able to manipulate data
bool parseEvents(const Json::Value& parsedData);
bool parseEvents(std::unique_ptr<rapidxml::xml_document<>> parsedData);
bool processEvent(const Json::Value& parsedEvent);
bool processEvent(rapidxml::xml_node<>* event);
std::pair<std::string, std::string> parseDescription(rapidxml::xml_node<>* description);
bool containsEvent(const Json::Value& events, const std::string& key);
bool containsEvent(rapidxml::xml_document<>& events, const std::string& key);
bool inMarket(const Json::Value& parsedEvent);
bool inRegionNY(const Json::Value& parsedEvent);
Region getRegionNY(const std::string& regionName);
bool isIncident(const Json::Value& parsedEvent);
std::chrono::system_clock::time_point getTime(const Json::Value& parsedEvent);
void deleteEvents(std::vector<std::string> keys);

// Define a template function for clearing events from the map
template<typename T>
void clearEvents(T& events) {
  // Create a vector to store the keys to be deleted
  std::vector<std::string> keysToDelete;
  // Iterate through the event map
  for(const auto& [key, event] : mapEvents) {
    // Only match with current source events
    if(event.getSource() == currentSource) {
      // Check for matching key
      if(!containsEvent(events, key)) {
        keysToDelete.push_back(key);
      std::cout << Output::Colors::YELLOW << "[Events] Marked event for deletion: " << key << Output::Colors::END << '\n'; 
      }
    }
  }
  deleteEvents(keysToDelete);
  currentSource = DataSource::UNKNOWN;
}

} // namespace Traffic

#endif
