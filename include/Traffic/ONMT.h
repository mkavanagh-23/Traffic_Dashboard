#ifndef ONMT_H
#define ONMT_H

#include <json/json.h>
#include <string>
#include <ostream>

namespace Traffic {
/******** Ontario Ministry of Transportation Traffic Data (511on.ca) **********/
namespace Ontario {

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
bool getEvents();
// And a function to parse events and store on the map
bool parseEvents(const Json::Value& events);
bool processEvent(const Json::Value& parsedEvent);
void printEvents();

class Camera {
private:
  int ID;
  std::string Source;
  std::string SourceID;
  std::string Roadway;
  std::string Direction;
  double Latitude;
  double Longitude;
  std::string location;
  Json::Value views;    // A JSON value objection containing all camera views at the location
  // TODO: Define a CameraView class that creates an object for each views
  // These can be stored in a std::vector rather than copying, storing, and reparsing the Json::Value object
public:
  Camera() = default;
};

bool getCameras();
bool parseCameras(const Json::Value &cameras);
bool processCamera(const Json::Value &parsedCamera);
} // namespace Ontario
} // namespace Traffic

#endif
