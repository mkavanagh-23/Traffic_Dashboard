#ifndef ONMT_H
#define ONMT_H

#include <json/json.h>
#include <string>
#include <ostream>
#include <vector>

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
void cleanEvents(const Json::Value& events);
bool containsEvent(const Json::Value& events, const std::string& key);
bool isIncident(const Json::Value& parsedEvent);
bool isConstruction(const Json::Value& parsedEvent);
void deleteEvents(const std::vector<std::string>& keys);
void printEvents();

class CameraView {
private:
  int ID;
  std::string URL;  // Link to a jpeg image of the latest camera still
                    //  No video stream links are available anymore, sadly
                    //  TODO: Implement JPEG stills conversion to SDL texture for inline rendering  
  std::string Status;
  std::string Description;
public:
  CameraView() = default;
  CameraView(const Json::Value& parsedView);

  std::string getURL(){ return URL; }
};

class Camera {
private:
  int ID;
  std::string Source;
  std::string SourceID;
  std::string Roadway;
  std::string Direction{ "Unknown" };
  double Latitude;
  double Longitude;
  std::string Location;
  std::vector<CameraView> Views;    // One or more camera streams located at the location
public:
  Camera() = default;
  // Construct an event from a Json object
  Camera(const Json::Value& parsedCamera);
  // Move constructor
  Camera(Camera&& other) noexcept;
  // Move assignment operator
  Camera& operator=(Camera&& other) noexcept;
};

bool getCameras();
bool parseCameras(const Json::Value &cameras);
bool processCamera(const Json::Value &parsedCamera);
} // namespace Ontario
} // namespace Traffic

#endif
