#include "MCNY.h"
#include "Output.h"
#include "Traffic.h"

#include <string>
#include <iostream>
#include <regex>
#include <tuple>

namespace Traffic {
namespace MCNY {

extern const std::string EVENTS_URL{ "https://www.monroecounty.gov/incidents911.rss" };

// Process an XML event for storage
bool processEvent(rapidxml::xml_node<>* parsedEvent) {
  // Extract Status and ID as a pair
  std::pair<std::string, std::string> description = parseDescription(parsedEvent->first_node("description"));
  auto& [status, key] = description;

  // Try to insert a new Event at event, inserted = false if it already exists
  auto [event, inserted] = mapEvents2.try_emplace(key, parsedEvent, description);
  // Check if we added a new event
  if(!inserted) {
    if(event->second.getStatus() != status) {
      event->second = Event2(parsedEvent, description);
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

// Process Title data element into event title, main street, direction (optional), and cross-street (optional) elements
std::optional<std::tuple<std::string, std::string, std::optional<std::string>, std::optional<std::string>>> processTitle(const std::string& title) {
  // Define the matching pattern
  std::regex pattern(R"(^(.+?) at\s+(?:\d+-?BLK\s+)?(?:\d+\s+)?@?((?:RR\s+|RT\s+)?(?:INNER\s+(EB|WB|NB|SB)\s+LOOP|LAKE\s+ONTARIO\s+(EB|WB|NB|SB)\s+STPK|[A-Z0-9]+)(?:\s+(?!(?:SWE|ROC|BRI|IRO|HEN|PEN|NYSP|MSO|PIT|GAT|HAM|CHI|WBT|GRE|OGD|HIL|BRO|PER)\b)(?!NB\b|SB\b|EB\b|WB\b)(?!MM\b)[A-Z0-9]+)*(?:\s+RD|ST|AVE|BLVD|PKWY|TRL|DR)?)(?:\s+(NB|SB|EB|WB))?(?:\s+MM\s+\d+(?:\.\d+)?)?(?:/(.+?))?(?:\s+(?:SWE|ROC|BRI|IRO|HEN|PEN|NYSP|MSO|PIT|GAT|HAM|CHI|WBT|GRE|OGD|HIL|BRO|PER)(?:\s+(?:SWE|ROC|BRI|IRO|HEN|PEN|NYSP|MSO|PIT|GAT|HAM|CHI|WBT|GRE|OGD|HIL|BRO|PER))?)?\s*(?:\([^)]*\))?\s*(?:EASTSIDE\s+RR)?(?::.*)?$)");
  std::smatch matches;
  /*
   *    matches[1] = event title
   *    matches[2] = mainStreet
   *    matches[3] = direction(if matched) [Optional]
   *        Special logic for inner loop
   *    matches[4] = direction(alternate path) [Optional]
   *        Special logic for LOSP
   *    matches[5] = direction (default path - if matched) [Optional]
   *    matches[6] = crossStreet (Optional)
   */
  
  if (std::regex_search(title, matches, pattern)) {

    // Extract the street name and direction
    std::string streetName{ matches[2] };
    bool hasCross{ matches[6].matched };  // check if we matched a cross-street

    if(matches[3].matched) {            // Inner Loop logic
      if(streetName.find("INNER") != std::string::npos)
        streetName = "INNER LOOP";
      if(hasCross)
        return std::make_tuple(matches[1], streetName, matches[3], matches[6]);
      else 
        return std::make_tuple(matches[1], streetName, matches[3], std::nullopt);
    } else if(matches[4].matched) {     // LOSP logic
      if(streetName.find("LAKE ONTARIO") != std::string::npos)
        streetName = "LAKE ONTARIO STPKWY";
      if(hasCross)
        return std::make_tuple(matches[1], streetName, matches[4], matches[6]);
      else
        return std::make_tuple(matches[1], streetName, matches[4], std::nullopt);
    } else if(matches[5].matched) {     // Defualt case logic
      if(hasCross)
        return std::make_tuple(matches[1], streetName, matches[5], matches[6]);
      else
        return std::make_tuple(matches[1], streetName, matches[5], std::nullopt);
    } else {
      if(hasCross)
        return std::make_tuple(matches[1], streetName, std::nullopt, matches[6]); 
      else
        return std::make_tuple(matches[1], streetName, std::nullopt, std::nullopt); 
    }
  }
  std::cerr << Output::Colors::RED << "[REGEX] ERROR: MCNY title does not match: " << matches[0] << '\n' << Output::Colors::END;
  return std::nullopt;
}
}
}
