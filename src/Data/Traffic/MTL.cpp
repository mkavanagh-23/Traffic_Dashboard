#include "MTL.h"
#include "Traffic.h"
#include <rapidxml.hpp>
#include <string>
#include <regex>

namespace Traffic {
namespace MTL {
extern const std::string EVENTS_URL{
  "https://www.quebec511.info/Diffusion/Rss/GenererRss.aspx?regn=13000&routes=10;13;15;19;20;25;40;112;117;125;134;136;138;335;520&lang=en"
};

// Process an XML event for storage
bool processEvent(rapidxml::xml_node<>* parsedEvent) {
  
  // Check for matching event type
  std::string eventType;
  if(rapidxml::xml_node<>* category = parsedEvent->first_node("category")){
    eventType = category->value();
  }
  if(eventType.find("Warning") == std::string::npos)
    return false;

  // Extract the url
  std::string url;
  if(rapidxml::xml_node<>* link = parsedEvent->first_node("link")){
    url = link->value();
  }
  // And the ID
  std::string id = extractID(url);
  if(id.empty()) {
    Output::logger.log(Output::LogLevel::WARN, "MTL", "Error parsing event ID");
    return false;
  }
  
  // Add the event to the map
  // Try to insert a new Event at event, inserted = false if it already exists
  auto [event, inserted] = mapEvents.try_emplace(id, parsedEvent);
  if(inserted)
    return true;

  return false;
}

std::string extractID(const std::string& url) {
  // Extract the ID from the URL
  size_t startPos = url.find("id=");
  if(startPos == std::string::npos) {
    Output::logger.log(Output::LogLevel::WARN, "MTL", "No id element found in link field");
    return "";
  }
  // Increment the iterator to the start of the id
  startPos += 3;
  // Find the end position, starting search from start position
  size_t endPos = url.find('&', startPos);
  // Check if we found end
  if(endPos == std::string::npos)
    endPos = url.length();
  // Extract the id substring
  return url.substr(startPos, endPos - startPos);
}

// Parse a title into a main roadway and eventType
std::optional<std::pair<std::string, std::optional<std::string>>> parseTitle(const std::string& title) {
  // Example string: "Roadway : EventType"
  // Define the matching pattern
  std::regex pattern(R"((.+)\s+:\s+(.+)?)");
  // matches[1] = roadway
  // matches[2] = eventType
  std::smatch matches;
  if(std::regex_search(title, matches, pattern)) {
    if(matches[2].matched)
      return std::make_pair(matches[1], matches[2]);
    else
      return std::make_pair(matches[1], std::nullopt);
  }
  return std::nullopt;
}

//void parseDescription(const std::string& description) {
//  std::vector<std::string> lines;
//  lines.reserve(6);
//  std::istringstream stream(description);
//  std::string line;
//
//  // Split the input string by line
//  while(std::getline(stream, line))
//    lines.push_back(line);
//
//  // Process each line from the lines vector
//  for(const auto& row : lines) {
//
//  }
//}
}
}
