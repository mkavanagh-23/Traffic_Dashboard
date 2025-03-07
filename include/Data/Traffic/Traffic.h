#ifndef TRAFFIC_H
#define TRAFFIC_H

// TODO:
// Use std::chrono to normalize to a common time type
// std::optional??, or at least think about how to handle null values and/or default types
#include "DataUtils.h"
#include <json/json.h>
#include <rapidxml.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace Traffic {

enum class Region {
  Syracuse,
  Rochester,
  Buffalo,
  Albany,
  Binghamton,
  Toronto,
  Ottawa
};

class Camera {
private:
  std::string ID;
  std::string webURL;
  std::string imageURL;
  std::string videoURL;
  bool online{ false };
  Region region;
  std::string roadwayName;
  std::string direction{ "Unknown" };
  Location location;

public:
  // Constructors
  Camera(const Json::Value& parsedCamera);
  Camera(Camera&& other) noexcept;
  Camera operator=(Camera&& other) noexcept;

  // Accessors
  std::string_view getID() const { return ID; }
  bool isOnline() const { return online; }
  Region getRegion() const { return region; }
  Location getLocation() const { return location; }
  std::string_view getStream() const { return videoURL; }
};

// Get cameras from all sources
void getCameras(std::string url);

class Event {
private:
  std::string ID;
  std::string URL;
  std::string status{ "Active" };
  Region region;
  std::string roadwayName;
  std::string directionOfTravel{ "Unknown" };
  std::string description;  // MCNY 'title' field
  std::string timeReported;
  std::string timeUpdated;
  Location location;
  std::vector<Camera> cameras;

public:
  // Constructors
  Event(const Json::Value& parsedEvent);
  Event(const rapidxml::xml_node<>* item, const std::pair<std::string, std::string> &description);
  Event(Event&& other) noexcept;
  Event& operator=(Event&& other) noexcept;

  // Accessors
  std::string_view getID() const { return ID; }
  std::string_view getStatus() const { return status; }
  Region getRegion() const { return region; }
  Location getLocation() const { return location; }
  std::string_view getDescription() const { return description; }
  const std::vector<Camera>& getCameras() const { return cameras; } // Return by const reference, ensure no dangling references!!!
};

// Get events from all sources
void getEvents(std::string url);

}

#endif
