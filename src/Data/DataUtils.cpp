#include "DataUtils.h"
#include "Output.h"
#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <chrono>
#include <curl/curl.h>
#include <json/json.h>
#include <rapidxml.hpp>

// Helper function to remove spaces and special characters from a string
std::string sanitizeString(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (std::isalnum(c)) {
            result += c;
        }
    }
    return result;
}

namespace cURL {

// Write the header data
size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
  size_t totalSize{ size * nitems };
  std::vector<std::string>* headers = static_cast<std::vector<std::string>*>(userdata);

  std::string header(buffer, totalSize);
  headers->push_back(header);

  return totalSize;
}

// Write the data to the string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
  size_t totalSize{ size * nmemb };    // Calculate the size of the callback buffer
  output->append((char*)contents, totalSize); // Store contents of callback buffer in output
  return totalSize;
}

// Fetch a data string from a remote source
std::tuple<Result, std::string, std::vector<std::string>> getData(const std::string& url){
  Handle curl;   // cURL malloc (Initialize an object via RAII)
  std::string responseData;         // Create a string to hold the data
  std::vector<std::string> headers; // Create a vector to hold the response headers
  
  // Check for successful initialization
  if(!curl) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to initialize cURL." << Output::Colors::END << '\n';
    return { Result::INIT_FAILED, "", {} };
  }
  
  // Set cURL options
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseData);     // Set the data to write
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
  // Set up header handling
  curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, HeaderCallback); // Custom function to capture headers
  curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &headers);           // Pass headers vector to function
    
  // Retrieve the data
  CURLcode res = curl_easy_perform(curl.get());

  // Check for errors
  if(res != CURLE_OK) {
    std::cerr << Output::Colors::RED << "[cURL] Error retrieving data: " << curl_easy_strerror(res) << ".\n";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "", {} };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "", {} };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "", {} };
    else
      return { Result::REQUEST_FAILED, "", {} };
  }
  return { Result::SUCCESS, responseData, headers };
}

// POST data to a remote endpoint
std::tuple<Result, std::string, std::vector<std::string>> postData(const std::string& url, const std::string& postData) {
  Handle curl;   // cURL malloc (RAII object)
  std::string responseData;         // Create a string to hold the data
  std::vector<std::string> headers; // Create a vector to hold the response headers
  
  // Check for successful initialization
  if(!curl) {
    std::cerr << Output::Colors::RED << "[cURL] Failed to initialize cURL." << Output::Colors::END << '\n';
    return { Result::INIT_FAILED, "", {} };
  }
  
  // Set cURL options
  curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str()); // Set the cURL url
  curl_easy_setopt(curl.get(), CURLOPT_POST, 1L); // Set the cURL POST method
  curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, postData.c_str()); // Set the POST data
  curl_easy_setopt(curl.get(), CURLOPT_TIMEOUT, 10L); // Set a 10 second timeout
  curl_easy_setopt(curl.get(), CURLOPT_SSL_VERIFYPEER, 0L); // Optional, depending on your SSL setup
  
  // Write the callback data 
  curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, WriteCallback); // Set the write function
  curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &responseData);     // Set the data to write

  // And write the header response
  curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, HeaderCallback); // Custom function to capture headers
  curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &headers);           // Pass headers vector to function

  // POST the data
  CURLcode res = curl_easy_perform(curl.get());
  
  // Check for errors
  if(res != CURLE_OK) {
    std::cerr << Output::Colors::RED << "[cURL] Error POSTing data: " << curl_easy_strerror(res) << ".\n";
    
    if(res == CURLE_UNSUPPORTED_PROTOCOL)
      return { Result::UNSUPPORTED_PROTOCOL, "", {} };
    else if(res == CURLE_URL_MALFORMAT)
      return { Result::BAD_URL, "", {} };
    else if(res == CURLE_OPERATION_TIMEDOUT)
      return { Result::TIMEOUT, "", {} };
    else
      return { Result::REQUEST_FAILED, "", {} };
  }
  return { Result::SUCCESS, responseData, headers };
}

// Extract the content-type header from the response
std::string getContentType(const std::vector<std::string>& headers) {
  // Iterate through each header
  for(const auto& header : headers) {
    // Check for the "Content-Type" header
    if(header.find("content-type") != std::string::npos || header.find("Content-Type") != std::string::npos) {
      // Set an iterator to the start of the value string
      size_t pos = header.find(":");
      // Check if the key has a value
      if(pos != std::string::npos) {
        return header.substr(pos + 1);  // Ectract the value that follows the ":"
      }
    }
  }
  return "";  // Return an empty string if the header wasn't found
}

} // namespace cURL

namespace JSON {
// Parse events from a Json data stream
Json::Value parseData(const std::string& jsonData) {
  // Set up Json parsing objects
  Json::CharReaderBuilder builder;
  Json::Value root;                 // Root node of the parsed objects
  std::istringstream data(jsonData);
  std::string errs;                 // Hold errors in a string

  // Parse the string into the root Value object
  if(!Json::parseFromStream(builder, data, &root, &errs)) {
    // If initial parsing fails, send an error message
    std::cerr << Output::Colors::RED << "[JSON] Parsing error (is it a valid stream?): " << errs 
              << '.' << Output::Colors::END << '\n';
    // TODO: Throw an exception if we do not parse from stream
    // Should also throw an exception in the underlying/preceding curl function
  }
  std::cout << Output::Colors::GREEN << "[JSON] Successfully parsed objects from data stream." 
            << Output::Colors::END << '\n';
  return root; // Return the parsed root of objects
}
} // namespace JSON

namespace XML {
// Parse events from an XML data stream
// NOTE: Use a unique ptr to ensure proper memory manegement here
// WARNING: The caller must ensure that xmlData remains valid for the lifetime of 
// the returned object as rapidxml does not copy the string but rather parses pointers 
// into the data string. Since the data is created at the start of the calling function,
// we should make sure parsedData remains scoped within it.
std::unique_ptr<rapidxml::xml_document<>> parseData(std::string& xmlData) {
  // Set up XML document object to hold the parsed data
  auto parsedData = std::make_unique<rapidxml::xml_document<>>();
  parsedData->parse<0>(xmlData.data());
  std::cout << Output::Colors::GREEN << "[XML] Successfully parsed document from data stream." 
            << Output::Colors::END << '\n';
  return parsedData;
}

} // namespace XML

// Create a unique key for the event object
void HTML::Event::createID() {
  std::string id{"ONGOV-"};
  // Start with the region code
  id += sanitizeString(region);
  // Add date component
  id += sanitizeString(date);
  // Add abridged agency
  std::string agen = sanitizeString(agency);
  if(agen.length() >= 3) {
    id += agen.substr(0, 3);
    id += agen.substr(agen.length() - 3, 3);
  } else 
    id += agen;
  // Add abridged title
  std::string titl = sanitizeString(title);
  if(titl.length() >= 3) {
    id += titl.substr(0, 3);
    id += titl.substr(titl.length() - 3, 3);
  } else
    id += titl;
  // Add abridged street
  std::string addr = sanitizeString(address);
  if(addr.length() >= 3) {
    id += addr.substr(0, 3);
    id += addr.substr(addr.length() - 3, 3);
  } else
    id += addr;
  // Add cross-street
  std::string cross = sanitizeString(xstreet);
  if(cross.length() >= 3) {
    id += cross.substr(0, 3);
    id += cross.substr(cross.length() - 3, 3);
  } else
    id += cross;
  // Convert to all upper
  std::transform(id.begin(), id.end(), id.begin(), [](unsigned char c){ return std::toupper(c); });
  // And move the string onto the object 
  ID = std::move(id);
}

namespace Traffic {
// Construct a location from two doubles
Location::Location(double latValue, double longValue) 
: latitude{ latValue }, longitude{ longValue }
{}

// Construct a location from a pair of latitude and longitude values
Location::Location(std::pair<double, double> coordinates) 
: Location( coordinates.first, coordinates.second )
{}

bool BoundingBox::contains(const Location& coordinate) const {
  auto& [latitude, longitude] = coordinate;
  if((latitude >= latBottom && latitude <= latTop) && (longitude >= longLeft && longitude <= longRight))
    return true;

  return false;
}

std::ostream &operator<<(std::ostream &out, const Location &location) {
  out << "Coordinate [ " << location.latitude << ", " << location.longitude << " ]";
  return out;
}
} // namespace Traffic

namespace Time {
using namespace std::chrono;

std::string offsetGMT{ "-0400" };

system_clock::time_point currentTime() {
  return system_clock::now();
}

std::time_t currentTime_t() {
  auto time = system_clock::to_time_t(currentTime());
  return time;
}

// Create a local formatted time string for printing from a time point object
std::tm toLocalPrint(const system_clock::time_point& time) {
  auto utcTime = system_clock::to_time_t(time);
  return *std::localtime(&utcTime);
}

// Convert a local timepoint to UTC
void toUTC(system_clock::time_point& timePoint, const std::string& offset) {
  int offsetHours = std::stoi(offset.substr(0, 3));
  int offsetMinutes = std::stoi(offset.substr(3, 2)) * (offsetHours < 0 ? -1 : 1);
  timePoint -= (hours(offsetHours) + minutes(offsetMinutes));
}

namespace UNIX {

system_clock::time_point toChrono(const int unixtime, std::optional<std::string> offset) {
  // Convert the given time into seconds since epoch
  seconds sinceEpoch(unixtime);
  // Create a seconds timepoint
  sys_seconds tp = sys_seconds{ sinceEpoch };
  // Convert to a system time point
  system_clock::time_point timePoint = time_point_cast<system_clock::duration>(tp);
  // Apply TZ offset if provided
  if(offset.has_value())
    toUTC(timePoint, offset.value());
  return timePoint;
}

} // namespace UNIX


namespace RFC2822 {

int stoiMonth3(const std::string& month) {
  if(month == "Jan")
    return 1;
  if(month == "Feb")
    return 2;
  if(month == "Mar")
    return 3;
  if(month == "Apr")
    return 4;
  if(month == "May")
    return 5;
  if(month == "Jun")
    return 6;
  if(month == "Jul")
    return 7;
  if(month == "Aug")
    return 8;
  if(month == "Sep")
    return 9;
  if(month == "Oct")
    return 10;
  if(month == "Nov")
    return 11;
  if(month == "Dec")
    return 12;

  // Else return an invalid number
  return -1;
}


std::optional<system_clock::time_point> toChrono(const std::string& rfc2822){
  // TODO: Modify to optionally take a 3-CHAR offset code in addition to a number offset
  // Regex matching for RFC2822
  std::regex pattern(R"((?:\w+, )?(\d{1,2}) (\w{3}) (\d{4}) (\d{2}):(\d{2}):(\d{2}) ([\+\-]\d{4})?)");
  // Create an object to store the matching pattern
  std::smatch matches;

  // Attempt to match the pattern
  if(std::regex_match(rfc2822, matches, pattern)) {
    // Extract matches and convert to appropriate types
    int parsedDay = std::stoi(matches[1]);     // 1 or 2-digit int
    int parsedMonth = stoiMonth3(matches[2]); // 3 chars string
    int parsedYear = std::stoi(matches[3]);   //4-digit int
    int parsedHours = std::stoi(matches[4]);   // 2-digit int
    int parsedMinutes = std::stoi(matches[5]);   // 2-digit int
    int parsedSeconds = std::stoi(matches[6]);   // 2-digit int

    // Create a local  Time Point object
    system_clock::time_point timePoint = sys_days(year_month_day(year(parsedYear), month(parsedMonth), day(parsedDay)))
                                       + hours(parsedHours) + minutes(parsedMinutes) + seconds(parsedSeconds);
    
    // Check for TZ offset in input string
    if(matches[7].matched) {
      // Convert timePoint to UTC
      toUTC(timePoint, matches[7]);
    }
    return timePoint;
  }
  return std::nullopt;
}
} // namespace RFC2822

namespace MMDDYYHHMM {

// Example string: "03/20/25 07:04"

system_clock::time_point toChrono(const std::string& timeStr) {
  int parsedMonth = std::stoi(timeStr.substr(0, 2));
  int parsedDay = std::stoi(timeStr.substr(3, 2));
  int parsedYear = std::stoi(timeStr.substr(6, 2)) + 2000;  // Adjust year for current century
  int parsedHours = std::stoi(timeStr.substr(9, 2));
  int parsedMinutes = std::stoi(timeStr.substr(12, 2));

  // Create a local time point object
  system_clock::time_point timePoint = sys_days(year_month_day(year(parsedYear), month(parsedMonth), day(parsedDay)))
                                     + hours(parsedHours) + minutes(parsedMinutes);
  
  // Apply GMT-offset to convert to UTC 
  toUTC(timePoint, offsetGMT);

  return timePoint;
}

} // namespace MMDDYYHHMM

namespace DDMMYYYYHHMMSS {

// Example string: "26/04/2025 10:30:00"

system_clock::time_point toChrono(const std::string& timeStr) {
  // Extract time components into ints
  int parsedDay = std::stoi(timeStr.substr(0, 2));   // 2-digits
  int parsedMonth = std::stoi(timeStr.substr(3, 2)); // 2-digits
  int parsedYear = std::stoi(timeStr.substr(6, 4));  // 4-digits
  int parsedHours = std::stoi(timeStr.substr(11, 2));  // 2-digits
  int parsedMinutes = std::stoi(timeStr.substr(14, 2));  // 2-digit
  int parsedSeconds = std::stoi(timeStr.substr(17, 2));    // 2-digit

  // Create a local time point object
  system_clock::time_point timePoint = sys_days(year_month_day(year(parsedYear), month(parsedMonth), day(parsedDay)))
                                     + hours(parsedHours) + minutes(parsedMinutes) + seconds(parsedSeconds);
  
  // Apply GMT-offset to convert to UTC 
  toUTC(timePoint, offsetGMT);

  return timePoint;
}
}

namespace YYYYMMDDHHMMSS {

// Example string: 2025-03-24 10:33:00

system_clock::time_point toChrono(const std::string& timeStr) {
  
  // Extract time components into ints
  int parsedYear = std::stoi(timeStr.substr(0, 4));   // 4-digits
  int parsedMonth = std::stoi(timeStr.substr(5, 2)); // 2-digits
  int parsedDay = std::stoi(timeStr.substr(8, 2));  // 2-digits
  int parsedHours = std::stoi(timeStr.substr(11, 2));  // 2-digits
  int parsedMinutes = std::stoi(timeStr.substr(14, 2));  // 2-digit
  int parsedSeconds = std::stoi(timeStr.substr(17, 2));    // 2-digit

  // Create a local time point object
  system_clock::time_point timePoint = sys_days(year_month_day(year(parsedYear), month(parsedMonth), day(parsedDay)))
                                     + hours(parsedHours) + minutes(parsedMinutes) + seconds(parsedSeconds);
  
  // Apply GMT-offset to convert to UTC 
  toUTC(timePoint, offsetGMT);

  return timePoint;
}

} // namespace YYYYMMDDHHMMSS
} // namespace Time
