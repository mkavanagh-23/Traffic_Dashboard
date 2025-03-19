#ifndef MCNY_H
#define MCNY_H

#include <rapidxml.hpp>
#include <string>

namespace Traffic {
namespace MCNY {
extern const std::string EVENTS_URL;

bool processEvent(rapidxml::xml_node<>* parsedEvent);
std::pair<std::string, std::string> parseDescription(rapidxml::xml_node<>* description);

}
}

#endif
