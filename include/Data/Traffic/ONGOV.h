#ifndef ONGOV_H
#define ONGOV_H

#include "DataUtils.h"

#include <string>
#include <vector>
#include <optional>
#include <utility>

namespace Traffic {
namespace ONGOV {
using addressDir = std::pair<std::string, std::optional<std::string>>;
extern const std::string EVENTS_URL;

namespace Gumbo {
// Parse data from HTML string
std::optional<std::vector<HTML::Event>>parseData(const std::string& htmlData);  
// Search parsed HTML for a table
void searchForTable(GumboNode* rootNode, const std::string& targetClass, std::vector<GumboNode*>& tables);
// Process a found table to extract data
std::optional<std::vector<HTML::Event>> processTable(GumboNode* tableNode);
// Process a table row into an Event and place on the vector
void processRow(GumboElement* tableRow, std::vector<HTML::Event>& eventsVector);
// Get the first span id from a table data element
std::string getFirstSpanId(GumboElement* tableData);
// Extract data from the table data element into the reference string
void getData(GumboElement* tableData, std::string& element);
}

// Process Address into and street name and (optional) direction
std::optional<addressDir> processAddress(const std::string& address);
// Process cross street value into main street and (optional) cross street
std::optional<std::pair<addressDir, std::optional<std::string>>> processCrossAsAddress(const std::string& address);

}
}

#endif
