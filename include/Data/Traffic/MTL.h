#ifndef MTL_H
#define MTL_H

#include <rapidxml.hpp>
#include <string>

namespace Traffic {
namespace MTL {
extern const std::string EVENTS_URL;  
bool processEvent(rapidxml::xml_node<>* parsedEvent);
}
}

#endif
