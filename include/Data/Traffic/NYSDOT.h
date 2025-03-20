#ifndef NYSDOT_H
#define NYSDOT_H

#include <jsoncpp/json/json.h>
#include <string>
#include "Traffic.h"

namespace Traffic {
namespace NYSDOT {
  extern std::string API_KEY;
  extern const std::string EVENTS_URL;
  extern const std::string CAMERAS_URL;
  extern const BoundingBox regionSyracuse;

void getEnv();
bool inRegion(const Json::Value& parsedEvent);
Region getRegion(const std::string& regionName);

}
}

#endif
