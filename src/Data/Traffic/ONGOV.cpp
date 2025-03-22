#include "ONGOV.h"
#include "Traffic.h"
#include "Output.h"
#include <gumbo.h>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <regex>

namespace Traffic {
namespace ONGOV {
extern const std::string EVENTS_URL{ "https://911events.ongov.net/CADInet/app/events.jsp" };

namespace Gumbo {
// Parse data from HTML string
std::optional<std::vector<HTML::Event>>parseData(const std::string& htmlData) {
  HTML::GumboOutputWrapper output(htmlData);

  if(!output) {
    std::cerr << Output::Colors::RED << "[HTML] ERROR: Failed to parse HTML content\n" << Output::Colors::END;
    return std::nullopt;
  }

  GumboNode* rootNode = output.root();

  // Check for valid HTML root node
  if(rootNode->type != GUMBO_NODE_ELEMENT) {
    std::cerr << Output::Colors::RED << "[HTML] ERROR: Root node is not an element node\n" << Output::Colors::END;
    return std::nullopt;
  }

  GumboElement* rootElement = &rootNode->v.element;

  // Verify that <html> is the root element
  if(rootElement->tag != GUMBO_TAG_HTML) {
    std::cerr << Output::Colors::RED << "[HTML] ERROR: Root tag is not <html>\n" << Output::Colors::END;
    return std::nullopt;
  }
  // Else Parsing Success!
  std::cout << Output::Colors::GREEN << "[HTML] HTML Parsing Success!\n" << Output::Colors::END;

  // Find the table with class "dataTableEx" and store them in a tables vector
  std::vector<GumboNode*> tables;
  searchForTable(rootNode, "dataTableEx", tables);

  // Check that we found a matching table
  if(tables.empty()) {
    std::cerr << Output::Colors::RED << "[HTML] ERROR: No matching tables found\n" << Output::Colors::END;
    return std::nullopt;
  }

  // Process each matching table in the vector (there should only be one)
  for(GumboNode* table : tables) {
    if(auto events = processTable(table)) {
      return *events;
    }
  }
  return std::nullopt;
}

// Search parsed HTML for a table
void searchForTable(GumboNode* rootNode, const std::string& targetClass, std::vector<GumboNode*>& tables) {
  // Check if the current node is an element node
  if(rootNode->type == GUMBO_NODE_ELEMENT) {
    // Extract the current element from the root node
    GumboElement* element = &rootNode->v.element;

    // Check if element is a <table> and for targetClass match
    if (element->tag == GUMBO_TAG_TABLE) {
      for (size_t i = 0; i < element->attributes.length; ++i) {
        GumboAttribute* attr = static_cast<GumboAttribute*>(element->attributes.data[i]);
        std::string attribute = attr->name;
        std::string value = attr->value;
        if (attribute == "class" && value == targetClass) {
          // If we find a match, store the table node
          std::cout << Output::Colors::GREEN << "[HTML] Found a matching table!\n" << Output::Colors::END;
          tables.push_back(rootNode);
          return;  // Stop searching since we found the target table
        }
      }
    }

    // If we didn't find the table, recursively search through each child node
    for (size_t i = 0; i < element->children.length; ++i) {
      searchForTable(static_cast<GumboNode*>(element->children.data[i]), targetClass, tables);
    }
  }
}

// Process a found table to extract data
std::optional<std::vector<HTML::Event>> processTable(GumboNode* tableNode) {

  // Create a vector to store our row vectors in
  std::vector<HTML::Event> tableData;

  // Iterate over each child node and look for table rows '<tr>'
  GumboElement* table = &tableNode->v.element;
  for (size_t i = 0; i < table->children.length; ++i) {
    GumboNode* sectionNode = static_cast<GumboNode*>(table->children.data[i]);
    if (!(sectionNode->type == GUMBO_NODE_ELEMENT))
      continue;

    GumboElement* section = &sectionNode->v.element;
    // Check for the table body
    if(!(section->tag == GUMBO_TAG_TBODY))
      continue;

    // Iterate over each <TR> in the table body
    for(size_t j = 0; j < section->children.length; ++j) {
      GumboNode* rowNode = static_cast<GumboNode*>(section->children.data[j]);
      if(!(rowNode->type == GUMBO_NODE_ELEMENT))
        continue;

      GumboElement* row = &rowNode->v.element;
      // Check for table row
      if(row->tag == GUMBO_TAG_TR)
        processRow(row, tableData);
    }
  }
  // Check if we have data to return
  if(!tableData.empty())
    return tableData;
  else
    return std::nullopt;
}

// Process a table row into an Event and place on the vector
void processRow(GumboElement* tableRow, std::vector<HTML::Event>& eventsVector) {
  // Create an empty event
  HTML::Event event;

  // Iterate over each table data element in the row
  for(size_t i = 0; i < tableRow->children.length; ++i) {
    GumboNode* cellNode = static_cast<GumboNode*>(tableRow->children.data[i]);
    if(!(cellNode->type == GUMBO_NODE_ELEMENT))
      continue;

    GumboElement* cell = &cellNode->v.element;
    // Check if our element is table data
    if(!(cell->tag == GUMBO_TAG_TD))
      continue;

    // Check the id of the first span to determine which element we have
    std::string spanID = getFirstSpanId(cell);
    if(spanID.empty())  // Check for valid span id extraction
      continue;
    if(spanID.find("text6") != std::string::npos) { // Extract the agency element
      getData(cell, event.agency);
      continue;
    }
    if(spanID.find("text12") != std::string::npos) { // Extract the date/time element
      getData(cell, event.date);
      continue;
    }
    if(spanID.find("textActiveevents_typ_desc1") != std::string::npos) { // Extract the title
      getData(cell, event.title);
      continue;
    }
    if(spanID.find("textActiveevents_edirpre1") != std::string::npos) { // Extract the address element
      getData(cell, event.address);
      continue;
    }
    if(spanID.find("textActiveevents_mun2") != std::string::npos) { // Extract the region element
      getData(cell, event.region);
      continue;
    }
    if(spanID.find("textActiveevents_xstreet11") != std::string::npos) { // Extract the cross street element
      getData(cell, event.xstreet);
      continue;
    }

  }
  // Check if the row contained data cells
  if(event.title != "N/A") {
    // Create an ID
    event.createID();
    eventsVector.push_back(event);
  }
}

// Get the first span id from a table data element
std::string getFirstSpanId(GumboElement* tableData) {
  // Check which span we are at at
  for(size_t i = 0; i < tableData->children.length; ++i) {
    GumboNode* spanNode = static_cast<GumboNode*>(tableData->children.data[i]);
    if(!(spanNode->type == GUMBO_NODE_ELEMENT))
      continue;

    GumboElement* span = &spanNode->v.element;
    // Check if our element is a span
    if(!(span->tag == GUMBO_TAG_SPAN))
      continue;

    GumboAttribute* idAttr = gumbo_get_attribute(&span->attributes, "id");
    if(idAttr)
      return idAttr->value; 
  }
  return "";
}

// Extract data from the table data element into the reference string
void getData(GumboElement* tableData, std::string& element) {
  std::string data;
  // Iterate through each child (<span>) object of our table data
  for(size_t i = 0; i < tableData->children.length; ++i) {
    GumboNode* spanNode = static_cast<GumboNode*>(tableData->children.data[i]);
    if(!(spanNode->type == GUMBO_NODE_ELEMENT))
      continue;

    GumboElement* span = &spanNode->v.element;
                  
    // Check if our element is a span
    if(!(span->tag == GUMBO_TAG_SPAN))
      continue;
                    
    // Extract text from the cell
    for(size_t j = 0; j < span->children.length; ++j) {
      GumboNode* textNode = static_cast<GumboNode*>(span->children.data[j]);
      // Check for text object                
      if(!(textNode->type == GUMBO_NODE_TEXT))
        continue;

      data += textNode->v.text.text;
    }
  }
  if(!data.empty()) {
    // Store the extracted string in the data element
    element = data;
  }
}

} // namespace Gumbo

// Process Address into and street name and (optional) direction
std::optional<std::pair<std::string, std::optional<std::string>>> processAddress(const std::string& address) {
  std::string cleanedAddress = address.substr(1);
    
  // Replace double spaces with single spaces first
  std::regex multipleSpace("\\s{2,}");
  cleanedAddress = std::regex_replace(cleanedAddress, multipleSpace, " ");
   
  // Define the matching pattern
  std::regex pattern("(?:(EB|WB|NB|SB) )?((?:ROUTE\\s+\\d+)|(?:I\\s+\\d+)|(?:[^\\s].*?\\s+(?:LN|ST|AVE|DR|CT|CANALWAY|PATH|BLVD|TER|KING|CIR|RD|STREET|ROWE|FARM|TRAIL|TPKE)))(\\s+.*)?");
  std::smatch matches;
  /*
   *    matches[1] = Direction (Optional)
   *    matches[2] = Road Name
   */
  // And match our regex
  if(std::regex_search(cleanedAddress, matches, pattern)) {
    if(matches[1].matched && !matches[1].str().empty())  // if we extracted a direction
      return std::make_pair(matches[2], matches[1]);
    else
      return std::make_pair(matches[2], std::nullopt);
  }
  
  std::cerr << Output::Colors::RED << "[REGEX] ERROR: ONGOV main street does not match: " << address << '\n' << Output::Colors::END;
  return std::nullopt;
}

// Process cross street value into main street and (optional) cross street
std::optional<std::pair<std::string, std::optional<std::string>>> processCrossAsAddress(const std::string& address) {
  // Define the matching pattern
  std::regex pattern("^(.*?)\\s*&\\s*(.*)?$");
  std::smatch matches;
  /*
   *            string start    = ^
   *            matches[1]      = (.*?)
   *            " & "           = \s*&\s*
   * (optional) matches[2]      = (.*)?
   *            string end      = $
   */
  if(std::regex_search(address, matches, pattern)) {
    if(matches[2].matched)
      return std::make_pair(matches[1], matches[2]);
    else
      return std::make_pair(matches[1], std::nullopt);
  }
  
  std::cerr << Output::Colors::RED << "[REGEX] ERROR: ONGOV cross street does not match" << address << '\n' << Output::Colors::END;
  return std::nullopt;
}

} // namespace ONGOV
} // namespace Traffic
