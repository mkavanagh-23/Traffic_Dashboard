#ifndef MTL_H
#define MTL_H

#include <rapidxml.hpp>
#include <string>

namespace Traffic {
namespace MTL {
extern const std::string EVENTS_URL;  
bool processEvent(rapidxml::xml_node<>* parsedEvent);
std::string extractID(const std::string& url);
void parseDescription(const std::string& description);
}
}

#endif
