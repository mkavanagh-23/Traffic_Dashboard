#include "DataUtils.h"
#include "Output.h"
#include "NYSDOT.h"
#include "Traffic.h"

#include <string>


namespace Traffic {
namespace NYSDOT {

std::string API_KEY{""};
extern const std::string EVENTS_URL{ "https://511ny.org/api/getevents?format=json&key=" };
extern const std::string CAMERAS_URL{ "https://511ny.org/api/getcameras?format=json&key=" };
extern constexpr BoundingBox regionSyracuse{ -76.562, -75.606, 43.553, 42.621 };

// Source data from local environment
void getEnv() {
  // Retrieve API Key from local environment
  const char* API_KEY = std::getenv("NYSDOT_API_KEY");

  // Check for valid sourcing
  if(API_KEY) {
    NYSDOT::API_KEY = API_KEY;
    Output::logger.log(Output::LogLevel::INFO, "ENV", "Successfully sourced 'NYSDOT_API_KEY' from local environment");
  } else
    Output::logger.log(Output::LogLevel::WARN, "ENV", "Failed to retrieve 'NYSDOT_API_KEY'");
}

// Check if the parsed event is within an NYSDOT region
bool inRegion(const Json::Value& parsedEvent) {
  std::string region = parsedEvent["RegionName"].asString();
  return (region == "Central Syracuse Utica Area" 
       || region == "Finger Lakes Rochester Area" 
       || region == "Niagara Buffalo Area" 
       || region == "Capital Region Albany Saratoga Area" 
       || region == "Southern Tier Homell Elmira Binghamton Area");
}

// Convert a region-name element to a Region object
Region getRegion(const std::string& regionName) {
  if(regionName == "Central Syracuse Utica Area")
    return Region::Syracuse;
  else if(regionName == "Finger Lakes Rochester Area")
    return Region::Rochester;
  else if(regionName == "Niagara Buffalo Area")
    return Region::Buffalo;
  else if(regionName == "Capital Region Albany Saratoga Area")
    return Region::Albany;
  else if(regionName == "Southern Tier Homell Elmira Binghamton Area")
    return Region::Binghamton;
  else
    return Region::UNKNOWN;
}

}
}
