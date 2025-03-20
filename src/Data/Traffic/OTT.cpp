#include "OTT.h"
#include "DataUtils.h"
#include <string>

namespace Traffic {
namespace OTT {
extern const std::string EVENTS_URL{ "https://traffic.ottawa.ca/service/events" };
extern constexpr BoundingBox regionOttawa{ -76.54, -74.73, 45.86, 44.86 }; 
}
}
