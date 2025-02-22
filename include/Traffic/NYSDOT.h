#ifndef NYSDOT_H
#define NYSDOT_H

#include <json/json.h>
#include <string>
#include <ostream>

namespace Traffic {
/********************* NYSDOT Traffic Data (511ny.org) ************************/

namespace NYSDOT {
extern std::string API_KEY;
bool getEnv();

// Define a NYSDOT::Event object
class Event {
private:    // Define all private members
  std::string ID;            // UNIQUE KEY
  std::string RegionName;          // The region where the event occurred.
  std::string CountyName;          // The county where the event occurred.
  std::string Severity{"Unknown"}; // Severity of the event
  std::string RoadwayName;         // The roadway on which the event occurred
  std::string DirectionOfTravel{"None"};
  /*One of: None, All Directions, Northbound, Eastbound, Southbound,
            Westbound, Inbound, Outbound, Both Directions.*/

  std::string Description;       // Type of event, dates, lanes affected
  std::string Location;          // Location on the roadway
  std::string LanesAffected;     // Describes the lane or number of lanes
  std::string LanesStatus;       // Describes the status of the lanes
  std::string PrimaryLocation;   // Starting point of affected roadway
  std::string SecondaryLocation; // End point of affected roadway
  std::string FirstArticleCity;  // City at starting point with article
  std::string SecondCity;        // City at ending point
  std::string EventType;
  /*One of: AccidentsAndIncidents, roadwork, specialEvents,
            closures, transitMode, generalInfo, winterDrivingIndex*/

  std::string EventSubType;       // Non-restrictive event type
  std::string MapEncodedPolyline; // Google Maps Winter condition lines
  std::string LastUpdated;        // "dd/MM/yyyy HH:mm:ss"
  double Latitude;                // between -90 and 90
  double Longitude;               // between -180 and 180
  std::string PlannedEndDate;     // "dd/MM/yyyy HH:mm:ss"
  std::string Reported;           // "dd/MM/yyyy HH:mm:ss"
  std::string StartDate;          // "dd/MM/yyyy HH:mm:ss"

public:
  // Constructors
  // Construct an Event from a Json object
  Event(const Json::Value& parsedEvent);
  // Move constructor
  Event(Event&& other) noexcept;
  // Move assignment operator
  Event& operator=(Event&& other) noexcept;

  std::string getLastUpdated() const { return LastUpdated; }

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
private:    // Define all private members
  std::string ID;
  std::string URL;  // URL to image
  std::string VideoURL; // URL to video stream
  std::string Name;
  std::string DirectionOfTravel;
  std::string RoadwayName;
  bool Disabled;
  bool Blocked;
  double Latitude;
  double Longitude;
public:
};

bool getCameras();
} // namespace NYSDOT
} // namespace Traffic
#endif
