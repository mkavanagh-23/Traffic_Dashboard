#ifndef OTT_H
#define OTT_H

#include "DataUtils.h"
#include <string>

namespace Traffic {
namespace OTT {
extern const std::string EVENTS_URL;
extern const BoundingBox regionOttawa;

std::optional<std::pair<double, double>> parseLocation(const std::string& coordinates);
}
}

#endif
