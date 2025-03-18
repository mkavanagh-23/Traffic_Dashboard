#include "DataUtils.h"

#include <string>


namespace Traffic {
namespace NYSDOT {
// API Key
std::string API_KEY{""};

// URL(s)
extern const std::string EVENTS_URL{ "https://511ny.org/api/getevents?format=json&key=" };
extern const std::string CAMERAS_URL{ "https://511ny.org/api/getcameras?format=json&key=" };

// Bounding Boxes
extern constexpr BoundingBox regionSyracuse{ -76.562, -75.606, 43.553, 42.621 };
}
}