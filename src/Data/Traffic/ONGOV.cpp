#include "ONGOV.h"
#include "DataUtils.h"
#include "Output.h"
#include "Traffic.h"
#include <cassert>
#include <gumbo.h>
#include <regex>
#include <string>
#include <utility>
#include <vector>
#include <optional>

namespace Traffic {
namespace ONGOV {

std::vector<std::string> payloads {
  "form1%3AtableEx1%3Aweb1__pagerWeb=0&form1%3AtableEx1%3Agoto1__pagerGoText=2&form1=form1",    // Page 1
  "form1%3AtableEx1%3Aweb1__pagerWeb=1&form1%3AtableEx1%3Agoto1__pagerGoText=1&form1=form1"     // Page 2
};

// TODO:
// Check for IP/origin restrictions on cURL requests
// Geo-blocked? Data center traffic?
// Investigate HERE traffic API for potential integration

extern const std::string EVENTS_URL{ "https://911events.ongov.net/CADInet/app/events.jsp" };

std::optional<std::pair<int, int>> getPageNumbers(const std::string& htmlData) {
  std::regex pattern(R"(Page (\d+) of (\d+))");
  std::smatch matches;

  if(std::regex_search(htmlData, matches, pattern)){
    return std::make_pair(std::stoi(matches[1].str()), std::stoi(matches[2].str()));
  } else {
    return std::nullopt;
  }
}

// Get data for all pages
bool postRequest(const std::string& url, int numPages, cURL::Handle& curlHandle) {
  assert(numPages <= payloads.size && "Number of HTML pages exceeds request payload size");
  std::string msg = "Found " + std::to_string(numPages) + " ONGOV HTML pages";
  Output::logger.log(Output::LogLevel::INFO, "cURL", msg);
  // Iterate over each page
  for(int i = 0; i < numPages; i++) {
    std::string loopMsg = "POST request to ONGOV page " + std::to_string(i + 1) + " of " + std::to_string(numPages);
    // Issue the POST request
    auto [result, data, headers] = cURL::postData(url, payloads[i], curlHandle);
    Output::logger.log(Output::LogLevel::INFO, "cURL", loopMsg);
    // Process the data
    // Check for successful extraction
    if(result == cURL::Result::SUCCESS) {
      // Make sure response data isnt empty
      if(!data.empty()) {
        processData(data, headers);
      }
    }
  }
  return true;
}

namespace Gumbo {
// Parse data from HTML string
std::optional<std::vector<HTML::Event>>parseData(const std::string& htmlData) {
  HTML::GumboOutputWrapper output(htmlData);

  if(!output) {
    Output::logger.log(Output::LogLevel::WARN, "HTML", "Failed to parse document");
    return std::nullopt;
  }

  GumboNode* rootNode = output.root();

  // Check for valid HTML root node
  if(rootNode->type != GUMBO_NODE_ELEMENT) {
    Output::logger.log(Output::LogLevel::WARN, "HTML", "Root node is not an element node");
    return std::nullopt;
  }

  GumboElement* rootElement = &rootNode->v.element;

  // Verify that <html> is the root element
  if(rootElement->tag != GUMBO_TAG_HTML) {
    Output::logger.log(Output::LogLevel::WARN, "HTML", "Root tag is not '<html>'");
    return std::nullopt;
  }
  // Else Parsing Success!
  Output::logger.log(Output::LogLevel::INFO, "HTML", "Successfully parsed document");

  // Find the table with class "dataTableEx" and store them in a tables vector
  std::vector<GumboNode*> tables;
  searchForTable(rootNode, "dataTableEx", tables);

  // Check that we found a matching table
  if(tables.empty()) {
    Output::logger.log(Output::LogLevel::WARN, "HTML", "No matching tables found");
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
          Output::logger.log(Output::LogLevel::INFO, "HTML", "Found a matching table");
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
      if(row->tag == GUMBO_TAG_TR) {
        processRow(row, tableData);
      }
    }
  }
  // Check if we have data to return
  if(!tableData.empty())
    return tableData;
  else {
    Output::logger.log(Output::LogLevel::WARN, "HTML", "No data rows found in table");
    return std::nullopt;
  }
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
      // text6 - Seems to be always empty
      // text7 - Responding agency
      getData(cell, event.agency);
      continue;
    }
    if(spanID.find("text12") != std::string::npos) { // Extract the date/time element
      // text12 - seems to be always empty
      // textActiveevents_mmdd1 - date and time string (mm/dd/yy HH::mm)
      getData(cell, event.date);
      continue;
    }
    if(spanID.find("textActiveevents_typ_desc1") != std::string::npos) { // Extract the title
      getData(cell, event.title);
      continue;
    }
    if(spanID.find("textActiveevents_edirpre1") != std::string::npos) { // Extract the address element
      getAddressData(cell, event);
      //getData(cell, event.address);
      continue;
    }
    if(spanID.find("textActiveevents_mun2") != std::string::npos) { // Extract the region element
      getData(cell, event.region);
      continue;
    }
    if(spanID.find("textActiveevents_xstreet11") != std::string::npos) { // Extract the cross street element
      getCrossData(cell, event);
      //getData(cell, event.xstreet);
      continue;
    }

  }
  // Check if the row contained data cells
  if(event.title != "") {
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
    // Trim whitespace
    trim(data);
    // Store the extracted string in the data element
    element = data;
  }
}

// Parse an address table data element into its sub-elements
void getAddressData(GumboElement* tableData, HTML::Event& event) {
  std::string dirPre, name, suff, dirPost, details;
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

      // Check the current span to determine which string to extract to
      GumboAttribute* idAttr = gumbo_get_attribute(&span->attributes, "id");
      std::string attribute = idAttr->value;
      if(attribute.find("edirpre1") != std::string::npos)
        dirPre = textNode->v.text.text;
      else if(attribute.find("efeanme1") != std::string::npos)
        name = textNode->v.text.text;
      else if(attribute.find("efeatyp1") != std::string::npos)
        suff = textNode->v.text.text;
      else if(attribute.find("edirsuf1") != std::string::npos)
        dirPost = textNode->v.text.text;
      else if(attribute.find("ecompl1") != std::string::npos)
        details = textNode->v.text.text;
      else
        Output::logger.log(Output::LogLevel::ERROR, "HTML", "Failed parsing ONGOV address (invalid <span> attribute)");
    }
  }
  // Check for successful extraction and store in HTML event
  if(!(dirPre.empty() || dirPre == " ")) {
    trim(dirPre);
    event.address += dirPre + ' ';
  }
  if(!(name.empty() || name == " ")) {
    trim(name);
    event.address += name;
    if(!(suff.empty() || suff == " ")) {
      trim(suff);
      event.address += ' ' + suff;
    }
  }
  if(!(dirPost.empty() || dirPost == " ")) {
    trim(dirPost);
    event.direction = dirPost;
  }
  if(!(details.empty() || details == " ")) {
    trim(details);
    event.details = details;
  }
}

// Parse an cross street table data element into its sub-elements
void getCrossData(GumboElement* tableData, HTML::Event& event) {
  std::string street1, street2, join;
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

      // Check the current span to determine which string to extract to
      GumboAttribute* idAttr = gumbo_get_attribute(&span->attributes, "id");
      std::string attribute = idAttr->value;
      if(attribute.find("xstreet11") != std::string::npos)
        street1 = textNode->v.text.text;
      else if(attribute.find("xstreet21") != std::string::npos)
        street2 = textNode->v.text.text;
      else if(attribute.find("text3") != std::string::npos)
        join = textNode->v.text.text;
      else
        Output::logger.log(Output::LogLevel::ERROR, "HTML", "Failed parsing ONGOV address (invalid <span> attribute)");
    }
  }
  // Check for successful extraction and store in HTML event
  if(!(street1.empty() || street1 == " ")) {
    trim(street1);
    event.xstreet1 += street1;
    event.xstreet += street1;
  }
  if(!(street2.empty() || street2 == " ")) {
    trim(street2);
    event.xstreet += ' ';
    if(!(join.empty() || join == " ")) {
      trim(join);
      event.xstreet += join + ' ';
    }
    event.xstreet2 += street2;
    event.xstreet += street2;
  }
}
} // namespace Gumbo

} // namespace ONGOV
} // namespace Traffic
