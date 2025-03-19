#include "ONMT.h"
#include "Traffic.h"
#include <string>

namespace Traffic {
namespace ONMT {
extern const std::string EVENTS_URL{ "https://511on.ca/api/v2/get/event?format=json&lang=en" };
extern constexpr BoundingBox regionToronto{ -80.099, -78.509, 44.205, 43.137 };

}
}
