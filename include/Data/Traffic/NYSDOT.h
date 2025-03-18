#ifndef NYSDOT_H
#define NYSDOT_H

#include <json/json.h>
#include <string>
#include "Traffic.h"

namespace Traffic {
namespace NYSDOT {

void getEnv();
bool inRegion(const Json::Value& parsedEvent);
Region getRegion(const std::string& regionName);

}
}

#endif
