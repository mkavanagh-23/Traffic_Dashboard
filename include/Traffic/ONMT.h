#ifndef ONMT_H
#define ONMT_H

#include "Data.h"

#include <json/json.h>
#include <string>
#include <ostream>

namespace Traffic {
/******** Ontario Ministry of Transportation Traffic Data (511on.ca) **********/
namespace Ontario {

// Define a bounding box to check for matching events
struct BoundingArea {
  static constexpr double longLeft{ -80.099 };
  static constexpr double longRight{ -78.509 };
  static constexpr double latTop{ 44.205 };
  static constexpr double latBottom{ 43.137 };

  // Check if the bounding area contains the given coordinate
  static bool contains(std::pair<double, double>& coordinate);
};

// Define an Ontario::Event object
class Event {
private:
  std::string ID;       // Unique key
  std::string Organization; // Providing organization
  std::string RoadwayName;
  std::string DirectionOfTravel{ "None" };
  /* One of: 
   *    None, All Directions, Northbound, Eastbound, Southbound,
   *    Westbound, Inbound, Outbound, Both Directions
  */
  std::string Description;
  int Reported;                 // Unix time
  int LastUpdated;              // Unix time
  int StartDate;                // Unix time
  int PlannedEndDate;           // Unix time
  std::string LanesAffected;
  double Latitude;
  double Longitude;
  double LatitudeSecondary;
  double LongitudeSecondary;
  std::string EventType;
  bool IsFullClosure;
  std::string Comment;
  std::string Recurrence;
  std::string RecurrenceSchedules;
  std::string EventSubType;
  std::string EncodedPolyline;
  std::string LinkId;
public:
  // Constructors
  // Construct an event from a Json object
  Event(const Json::Value& parsedEvent);
  // Move constructor
  Event(Event&& other) noexcept;
  // Move assignment operator
  Event& operator=(Event&& other) noexcept;

  int getLastUpdated() const { return LastUpdated; }
  
  // Overload the ostream operator
  friend std::ostream &operator<<(std::ostream &out, const Event &event);
};

// Declare a hashmap to store NYSDOT::Event objects
extern TrafficMap<Event> eventMap; // Index into the map via "ID"
bool getEvents();
// And a function to parse events and store on the map
bool parseEvents(const Json::Value& events);
bool processEvent(const Json::Value& parsedEvent);
void printEvents();

} // namespace Ontario
} // namespace Traffic

#endif
