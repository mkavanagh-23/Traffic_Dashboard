#ifndef OTT_H
#define OTT_H

#include "DataUtils.h"
#include <string>
#include <optional>
#include <tuple>

namespace Traffic {
namespace OTT {
extern const std::string EVENTS_URL;
extern const BoundingBox regionOttawa;

using roadwayStr = std::tuple<std::string, std::optional<std::string>, std::optional<std::string>>;

std::optional<std::pair<double, double>> parseLocation(const std::string& coordinates);
std::optional<roadwayStr> parseHeadline(const std::string& headline);
}
}

#endif
