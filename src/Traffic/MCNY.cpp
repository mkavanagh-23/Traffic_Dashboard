#include "Traffic/MCNY.h"
#include "Data.h"
#include "Output.h"

#include <string>
#include <iostream>
#include <rapidxml.hpp>

namespace Traffic {
/************************ Monroe County Dispatch Feed *************************/

namespace MCNY {
const std::string RSS_URL{ "https://www.monroecounty.gov/incidents911.rss" };
EventMap<Event> eventMap; // Key = "ID"

bool getEvents() {
  // Parse Events Data from RSS feed
  std::string responseStr{ cURL::getData(RSS_URL) };
  if(responseStr.empty()) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to retrieve XML from RSS feed." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[cURL] Successfully retrieved XML from RSS feed." << Output::Colors::END << '\n';

  // Test XML parsing
  rapidxml::xml_document<> parsedData;  // Create a document object to hold XML events
  XML::parseData(parsedData, responseStr);
  if(!parseEvents(parsedData)) {
    std::cerr << Output::Colors::RED << "[XML] Error parsing root tree." << Output::Colors::END << '\n';
    return false;
  }
  std::cout << Output::Colors::GREEN << "[XML] Successfully parsed root tree." << Output::Colors::END << '\n';

  return true;
}

bool parseEvents(rapidxml::xml_document<>& xml) { 
  rapidxml::xml_node<>* root = xml.first_node("rss"); // Define root entry point
  rapidxml::xml_node<>* channel = root->first_node("channel"); // Navigate to channel
  
  // Iterate throgh each event in the document tree
  for(rapidxml::xml_node<>* item = channel->first_node("item"); item; item = item->next_sibling()) {
    // Create a temporary event object
    Event event(item);
    eventMap.insert_or_assign(event.getID(), std::move(event));
    // TODO:
    // Log time to track LastUpdated
  }
  std::cout << "Found " << eventMap.size() << " Matching Event Records.\n";

  return true;
}

// Print the event map
void printEvents() {
  for(const auto& [key, event] : eventMap) {
    std::cout << event << '\n';
  }
}


/***************************** MCNY EVENT *************************************/
//Construct an event from a parsed XML item

Event::Event(const rapidxml::xml_node<>* item) {
  if(rapidxml::xml_node<> *title = item->first_node("title")) {
    Title = title->value();
  }
  if(rapidxml::xml_node<> *link = item->first_node("link")) {
    Link = link->value();
  }
  if(rapidxml::xml_node<> *pubDate = item->first_node("pubDate")) {
    PubDate = pubDate->value();
  }
  if(rapidxml::xml_node<> *guid = item->first_node("guid")) {
    GUID = guid->value();
  }
  if(rapidxml::xml_node<> *latitude = item->first_node("geo:lat")) {
    std::string temp = latitude->value();
    Latitude = std::stof(temp.substr(1));
  }
  if(rapidxml::xml_node<> *longitude = item->first_node("geo:long")) {
    std::string temp = longitude->value();
    Longitude = std::stof(temp.substr(1));
  }
  // Get the Status and ID
  if(rapidxml::xml_node<> *description = item->first_node("description")) {
    // Extract items from the description
    std::stringstream ss(description->value());
    std::string token;
    std::vector<std::string> tokens;
    
    // Elements delimited by ','
    while(std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }
    
    // First token is status
    Status = tokens[0].substr(tokens[0].find(":") + 2);
    // Second token is ID
    ID = tokens[1].substr(tokens[1].find(":") + 2);
  }
    
  std::cout << Output::Colors::YELLOW << "Constructed MCNY event: " << ID << Output::Colors::END << '\n';
}

  // Move constructor
Event::Event(Event&& other) noexcept
: ID(std::move(other.ID)),
  Title(std::move(other.Title)),
  Link(std::move(other.Link)),
  PubDate(std::move(other.PubDate)),
  Status(std::move(other.Status)),
  GUID(std::move(other.GUID)),
  Latitude(other.Latitude),
  Longitude(other.Longitude)
{
  std::cout << Output::Colors::BLUE << "Moved MCNY event: " << ID << Output::Colors::END << '\n';
}
  
// Move assignment operator
Event& Event::operator=(Event&& other) noexcept {
  // Check for self assignment
  if (this != &other) {
    ID = std::move(other.ID);
    Title = std::move(other.Title);
    Link = std::move(other.Link);
    PubDate = std::move(other.PubDate);
    Status = std::move(other.Status);
    GUID = std::move(other.GUID);
    Latitude = other.Latitude;
    Longitude = other.Longitude;
  }
  std::cout << Output::Colors::BLUE << "[MCNY] Invoked move assignment: " << Output::Colors::END << '\n';
  return *this;
}

std::ostream &operator<<(std::ostream &out, const Event &event) {
  out << "\nEvent ID: " << event.ID << "  |  " << event.Status
      << '\n' << event.Title
      << "\nDate: " << event.PubDate
      << std::endl;
  return out;
}
} // namespace MCNY
}
