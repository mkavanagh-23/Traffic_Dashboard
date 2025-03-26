#ifndef MTL_H
#define MTL_H

#include <rapidxml.hpp>
#include <string>
#include <optional>

namespace Traffic {
namespace MTL {
extern const std::string EVENTS_URL;  
bool processEvent(rapidxml::xml_node<>* parsedEvent);
std::string extractID(const std::string& url);
std::optional<std::pair<std::string, std::optional<std::string>>> parseTitle(const std::string& title);
void parseDescription(const std::string& description);
}
}

#endif
