#ifndef MCNY_H
#define MCNY_H

#include <rapidxml.hpp>
#include <string>
#include <optional>
#include <tuple>

namespace Traffic {
namespace MCNY {
extern const std::string EVENTS_URL;

bool processEvent(rapidxml::xml_node<>* parsedEvent);
std::pair<std::string, std::string> parseDescription(rapidxml::xml_node<>* description);

// Process title into and street name and (optional) direction
std::optional<std::tuple<std::string, std::string, std::optional<std::string>, std::optional<std::string>>> processTitle(const std::string& address);
}
}

#endif
