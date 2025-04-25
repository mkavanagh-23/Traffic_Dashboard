#include "NYSTA.h"
#include "DataUtils.h"
#include "Output.h"
#include "Traffic.h"

#include <rapidxml.hpp>
#include <string>

namespace Traffic {
namespace NYSTA {

const std::string EVENTS_URL{ "https://www.thruway.ny.gov/xml/netdata/events.xml" };

// Process an XML event for storage
bool processEvent(rapidxml::xml_node<>* parsedEvent, const std::string& timeUpdated) {   
  // Extract the key
  std::string key = XML::getAttrValue(parsedEvent, "eventid"); 
  processedKeys.push_back(key);

  // TODO:
  // Convert the time to a std::chrono::system_clock::time_point

  // Try to insert a new Event, inserted = false if it already exists
  auto [event, inserted] = mapEvents.try_emplace(key, parsedEvent);
  
  return true;
}

} //namespace NYSTA
} //namespacd Traffic
