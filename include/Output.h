#ifndef OUTPUT_H
#define OUTPUT_H

#include <ctime>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

namespace Output {

enum class LogLevel {
  INFO,
  ERROR,
  WARN,
  DEBUG,
  OTHER
};

std::string toString(const LogLevel& level);
bool createDirIfMissing(const std::string& filePath);

class Logger {
private:
  std::string path;
  std::ofstream logFile;
  std::mutex logMutex;
public:
  Logger(const::std::string& fileName);
  ~Logger();
  
  // Prevent copying to avoid multiple objects managing the same file
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  void flush();

  template<typename T>
  void log(const LogLevel level, const std::string& type, const T& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if(!logFile.is_open() || !logFile) {
      // Handle error case
      if(!createDirIfMissing(path))
        return;
      else
        logFile.open(path, std::ios::app);
    }

    // Create a stringstream to hold the message
    std::stringstream ss;
    
    // Log the level
    ss << "[" << toString(level) << "] ";

    // Log the current time
    auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    ss << std::put_time(std::localtime(&time), "[%Y-%m-%d %H:%M:%S] ");

    // Log the message
    ss << type << ": " << message << '\n';

    // Push the stream to the file
    std::string logMessage = ss.str();
    logFile << logMessage;
  }
};

extern Logger logger;

namespace Colors {
  const std::string RED = "\033[31m";
  const std::string GREEN = "\033[32m";
  const std::string YELLOW = "\033[33m";
  const std::string BLUE = "\033[34m";
  const std::string MAGENTA = "\033[35m";
  const std::string CYAN = "\033[36m";
  const std::string END = "\033[0m";
}
}

#endif
