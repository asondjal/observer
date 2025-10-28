#ifndef INC_UTILS_LOGGING_HPP_
#define INC_UTILS_LOGGING_HPP_

#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace observer::logging {

enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

class Logging {
 private:
  std::ofstream logFile_;
  std::mutex logMutex_;
  LogLevel currentLevel_;
  bool consoleOutput_;

  std::string GetTimestamp() const;
  std::string LevelToString(LogLevel level) const;

 public:
  Logging(const std::string& logFilePath, LogLevel level = LogLevel::INFO,
          bool consoleOutput = true);
  ~Logging();

  void Log(const std::string& message, LogLevel level = LogLevel::INFO);
  void SetLevel(LogLevel level);
  void EnableConsoleOutput(bool enable);
  bool TestConsoleStatus();
  void TestGetTimestamp();
  std::string GetLevel();
};
}  // namespace observer::logging

#endif  // INC_UTILS_LOGGING_HPP_
