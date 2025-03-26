#include "MTL.h"
#include "DataUtils.h"
#include "Traffic.h"
#include <chrono>
#include <rapidxml.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace Traffic {
namespace MTL {
extern const std::string EVENTS_URL{ "https://www.quebec511.info/Diffusion/Rss/GenererRss.aspx?regn=13000&routes=10;13;15;19;20;25;40;112;117;125;134;136;138;335;520&lang=en" };

// Process an XML event for storage
bool processEvent(rapidxml::xml_node<>* parsedEvent) {
  
  if(rapidxml::xml_node<>* title = parsedEvent->first_node("title")){
    std::string eventTitle = title->value();
    // "Roadway : EventType"
  }

  if(rapidxml::xml_node<>* description = parsedEvent->first_node("description")){
    // TODO:
    // Parse description into several elements
    // Elements are delimited via new line so should be relatively simple
    auto parsedDescription = parseDescription(description->value());
    /*
     * Line 1       Town name
     * Line 2       Main Roadway
     * Line 3       Between [CROSS] and [CROSS]
     * Line 4       Details (lanes/closure)
     * Line 5
     *
     *
     * */
  }

  if(rapidxml::xml_node<>* pubDate = parsedEvent->first_node("pubDate")){
    // TODO:
    // Should be converted from RFC2822, need to slightly refine our regex matching first
    std::string date = pubDate->value();
    auto timeOpt = Time::RFC2822::toChrono(date);
    std::chrono::system_clock::time_point time;
    if(timeOpt)
      time = *timeOpt;
  }

  if(rapidxml::xml_node<>* link = parsedEvent->first_node("link")){
    std::string url = link->value();
  }

  if(rapidxml::xml_node<>* category = parsedEvent->first_node("category")){
    // TODO:
    // Extract this value first to determine whether we are of a desired type
    // Other parsing can likely be handled via the event constructor
    std::string eventType = category->value();
  }

  // Process the elements of an XML <item>
  //    <title>
  //    <description>   parse to lots of info - Should build a print program to collect lots of data
  //    <pubDate>       Fri, 21 Mar 2025 20:56:29 GMT
  //    <link>          endpoint
  //    <category>      Filter value


  // TODO:
  // Define logic for adding a new event to the map

  //// Try to insert a new Event at event, inserted = false if it already exists
  //auto [event, inserted] = mapEvents.try_emplace(key, parsedEvent, description);
  //// Check if we added a new event
  //if(!inserted) {
  //  if(event->second.getStatus() != status) {
  //    event->second = Event(parsedEvent, description);
  //    std::cout << Output::Colors::MAGENTA << "[XML] Updated event: " << key << Output::Colors::END << '\n';
  //    return true;
  //  }
  //  return false;
  //}

  return true;
}

void parseDescription(const std::string& description) {
  std::vector<std::string> lines;
  lines.reserve(6);
  std::istringstream stream(description);
  std::string line;

  // Split the input string by line
  while(std::getline(stream, line))
    lines.push_back(line);

  // Process each line from the lines vector
  for(const auto& row : lines) {

  }
}
}
}
