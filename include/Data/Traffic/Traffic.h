#ifndef TRAFFIC_H
#define TRAFFIC_H

#include "DataUtils.h"
#include <iostream>
#include <memory>
#include <json/json.h>
#include <rapidxml.hpp>
#include <mutex>
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
  Montreal,
  UNKNOWN
};
std::ostream& operator<<(std::ostream& out, const Region& region);
Region toRegion(const std::string& regionStr);

enum class DataSource {
  NYSDOT,
  ONGOV,
  MCNY,
  ONMT,
  OTT,
  MTL,
  UNKNOWN
};
std::ostream& operator<<(std::ostream& os, const DataSource& dataSource);
std::string toString(const DataSource& dataSource);
DataSource toSource(const std::string& sourceStr);
void setSource(const DataSource source);

extern DataSource currentSource;
//extern std::string currentCookie;

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
  std::string URL{ "N/A" };
  DataSource dataSource{ DataSource::UNKNOWN };
  Region region{ Region::UNKNOWN };
  std::string title{ "N/A" };
  std::string status{ "Active" };
  std::string mainStreet{ "N/A" };
  std::string crossStreet{ "N/A" };
  std::string direction{ "N/A" };
  std::string description{ "N/A" }; // Holds full unformatted event string
  Location location{ 0, 0 };
  std::chrono::system_clock::time_point timeReported; // OTT 'created' "2025-03-11 12:45:00"
  std::chrono::system_clock::time_point timeUpdated;  // OTT 'updated' "2025-03-11 12:45:00"
  bool printed{ false };
public:
  // Constructors
  Event(const Json::Value& parsedEvent);
  Event(const rapidxml::xml_node<>* item, const std::pair<std::string, std::string> &description);
  Event(const rapidxml::xml_node<>* item);
  Event(const HTML::Event& parsedEvent);
  Event(Event&& other) noexcept;

  // Operators
  Event& operator=(Event&& other) noexcept;
  friend std::ostream &operator<<(std::ostream &out, const Event& event);

  // Accessors
  void print();
  bool hasPrinted() const { return printed; }
  std::string_view getID() const { return ID; }
  DataSource getSource() const { return dataSource; }
  std::string_view getStatus() const { return status; }
  std::chrono::system_clock::time_point getLastUpdated() const { return timeUpdated; }
  Region getRegion() const { return region; }
  Location getLocation() const { return location; }
  std::pair<double, double> getCoordinates() const { return std::make_pair(location.latitude, location.longitude); }
  std::string_view getDescription() const { return description; }

  // Rest API
  // Serialize a traffic event into a Json object
  void serializeToJSON(Json::Value& item) const;
};

// Define extern event data structures
extern std::mutex eventsMutex;
extern std::unordered_map<std::string, Event> mapEvents;

// And deletion data
extern std::vector<std::string> processedKeys;

// Get events from all sources
void fetchEvents();
void fetchCameras();
void printEvents();
void printEvents(Region region);
bool getEvents(std::string url);
bool processData(std::string& data, const std::vector<std::string>& headers);   // XML must be able to manipulate data
bool parseEvents(const Json::Value& parsedData);
bool parseEvents(std::unique_ptr<rapidxml::xml_document<>> parsedData);
bool parseEvents(const std::vector<HTML::Event>& parsedData);
bool processEvent(const Json::Value& parsedEvent);
bool inMarket(const Json::Value& parsedEvent);
Location getLocation(const Json::Value& parsedEvent);
bool isIncident(const Json::Value& parsedEvent);
std::chrono::system_clock::time_point getTime(const Json::Value& parsedEvent);
void clearEvents();
void deleteEvents(const std::vector<std::string>& keys);
std::optional<Json::Value> serializeEventsToJSON(const std::vector<std::pair<std::string, std::string>>& queryParams);

} // namespace Traffic

#endif
