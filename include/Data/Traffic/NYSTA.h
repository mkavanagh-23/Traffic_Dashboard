#ifndef NYSTA_H
#define NYSTA_H

#include <rapidxml.hpp>
#include <string>

namespace Traffic {
namespace NYSTA {
extern const std::string EVENTS_URL;

bool processEvent(rapidxml::xml_node<>* parsedEvent, const std::string& timeUpdated);   

} // namespace NYSTA
} //namespace Traffic


#endif
