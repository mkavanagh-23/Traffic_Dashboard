#ifndef MCNY_H
#define MCNY_H

#include "Data.h"

#include <rapidxml.hpp>
#include <string>
#include <ostream>

namespace Traffic {
namespace MCNY {

// Define a MCNY::Event object
class Event {
private:
  std::string ID;   // UNIQUE KEY
  std::string Title;
  std::string Link;
  std::string PubDate;
  std::string Status;
  /*One of: WAITING, DISPATCHED, ENROUTE, ONSCENE*/

  std::string GUID;
  double Latitude;
  double Longitude;

public:
  // Constructors
  // Construct an event from an XML object
  Event(const rapidxml::xml_node<>* item);
  std::string getID(){ return ID; }
  // Move constructor
  Event(Event&& other) noexcept;
  // Move assignment operator
  Event& operator=(Event&& other) noexcept;

  // Overload the ostream operator
  friend std::ostream &operator<<(std::ostream &out, const Event &event);

public:
  // Accessots (Getters)
  // Modifiers (Setters)
};

// Declare a hashmap to store MCNY::Event objects
extern EventMap<Event> eventMap; // Index into the map via "ID"
extern const std::string RSS_URL;
bool getEvents();
bool parseEvents(rapidxml::xml_document<>& xml);
void printEvents();

} // namespace MCNY
}

#endif
