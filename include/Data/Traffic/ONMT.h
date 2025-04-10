#ifndef ONMT_H
#define ONMT_H

#include "DataUtils.h"
#include <string>
#include <optional>
#include <tuple>

namespace Traffic {
namespace ONMT {
extern const std::string EVENTS_URL;
extern const BoundingBox regionToronto;
extern const BoundingBox regionOttawa;

std::optional<std::tuple<std::string, std::string, std::string>> parseDescription(const std::string& description);
}
}


#endif
