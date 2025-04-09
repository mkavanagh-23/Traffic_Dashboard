#include "Output.h"
#include <filesystem>
#include <system_error>
#include <string>
#include <ios>
#include <iostream>

namespace Output {
Logger logger("log.txt");
Logger mtlLog("logs/mtl_log.txt");
Logger ontLog("logs/ont_log.txt");
Logger ottLog("logs/ott_log.txt");

std::string toString(const LogLevel& level) {
  switch(level) {
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::ERROR:
      return "ERROR";
    case LogLevel::WARN:
      return "WARN";
    case LogLevel::DEBUG:
      return "DEBUG";
    default:
      return "N/A";
  }
}

bool createDirIfMissing(const std::string& filePath) {
  std::filesystem::path path(filePath);
  auto dir = path.parent_path();

  if (dir.empty()) {
    return true; // No directory part, just a filename
  }

  std::error_code ec;
  if (!std::filesystem::exists(dir, ec)) {
    return std::filesystem::create_directories(dir, ec);
  }  
  
  return !ec; // Returns true if no error occurred
}

void clearConsole() {
  // ANSI escape code to clear screen and move cursor to home position
  std::cout << "\033[2J\033[1;1H";
}

Logger::Logger(const std::string& fileName) 
: path{fileName}
{
  // Check if path exists
  createDirIfMissing(path);
  // Open the logfile for writing
  logFile.open(path, std::ios::app);
}

Logger::~Logger() {
  if(logFile.is_open()) {
    logFile.flush();
    logFile.close();
  }
}

void Logger::flush() {
  std::lock_guard<std::mutex> lock(logMutex);
  if(logFile.is_open())
    logFile.flush();
}

}
