#ifndef NYSDOT_H
#define NYSDOT_H

#include <string>
#include "Traffic.h"

#ifdef JSONCPP_WITH_SUBDIR
#include <jsoncpp/json/json.h>
#else
#include <json/json.h>
#endif

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
