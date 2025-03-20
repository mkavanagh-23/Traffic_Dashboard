#include "MCNY.h"
#include "Output.h"
#include "Traffic.h"

#include <string>
#include <iostream>

namespace Traffic {
namespace MCNY {

extern const std::string EVENTS_URL{ "https://www.monroecounty.gov/incidents911.rss" };

// Process an XML event for storage
bool processEvent(rapidxml::xml_node<>* parsedEvent) {
  // Extract Status and ID as a pair
  std::pair<std::string, std::string> description = parseDescription(parsedEvent->first_node("description"));
  auto& [status, key] = description;

  // Try to insert a new Event at event, inserted = false if it already exists
  auto [event, inserted] = mapEvents.try_emplace(key, parsedEvent, description);
  // Check if we added a new event
  if(!inserted) {
    if(event->second.getStatus() != status) {
      event->second = Event(parsedEvent, description);
      std::cout << Output::Colors::MAGENTA << "[XML] Updated event: " << key << Output::Colors::END << '\n';
      return true;
    }
    return false;
  }
  return true;
}

// Parse status and ID from event description field
std::pair<std::string, std::string> parseDescription(rapidxml::xml_node<>* description) {
  // Extract items from the description
  std::stringstream ss(description->value());
  std::string token;
  std::vector<std::string> tokens;
  
  // Elements delimited by ','
  while(std::getline(ss, token, ',')) {
    tokens.push_back(token);
  }

  // First token is status
  // Second token is ID
  return { tokens[0].substr(tokens[0].find(":") + 2), 
           tokens[1].substr(tokens[1].find(":") + 2) };
}

}
}
