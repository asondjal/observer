#include "utils/logging.hpp"

namespace observer::logging {
/**
 * @brief Constructor of Logging
 * @param logFilePath Path to the log file
 * @param level Initial LogLevel state which has to be set
 * @param consoleOutput
 */
Logging::Logging(const std::string& logFilePath, LogLevel level, bool consoleOutput)
    : currentLevel_(level), consoleOutput_(consoleOutput) {
  logFile_.open(logFilePath, std::ios::app);
  if (!logFile_.is_open()) {
    std::cerr << "[ERROR] Unable to open log file: " << logFilePath << std::endl;
  }
}
/**
 * @brief Destructor of Logging and closure of logging file
 */
Logging::~Logging() {
  if (logFile_.is_open()) {
    logFile_.close();
  }
}

/**
 * @brief Setting current status
 * @param level Loglevel which will be the new state
 */
void Logging::SetLevel(LogLevel level) {
  std::lock_guard<std::mutex> lock(logMutex_);
  currentLevel_ = level;
}

/**
 * @brief Enabling console output
 * @param enable Boolean in order to allow the display of the current LogLevel state
 */
void Logging::EnableConsoleOutput(bool enable) {
  std::lock_guard<std::mutex> lock(logMutex_);
  consoleOutput_ = enable;
}

/**
 * @brief Function in order to monitor the console output
 */
bool Logging::TestConsoleStatus() {
  EnableConsoleOutput(true);
  bool statusTrue = consoleOutput_ == true;

  EnableConsoleOutput(false);
  bool statusFalse = consoleOutput_ == false;

  return statusTrue && statusFalse;
}

/**
 * @brief Current Time
 * @return Time in format `TT:MM:YY H:M:S`
 */
std::string Logging::GetTimestamp() const {
  auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now);
  std::ostringstream ss;
  ss << std::put_time(std::localtime(&itt), "%d.%m.%Y %H:%M:%S");
  return ss.str();
}

void Logging::TestGetTimestamp() {
  Logging logger("logfile.txt", LogLevel::DEBUG);
  std::string timestamp = logger.GetTimestamp();

  // Check basic format: DD.MM.YYYY HH:MM:SS (length should be 19)
  assert(timestamp.length() == 19);
  assert(timestamp[2] == '.' && timestamp[5] == '.' && timestamp[10] == ' ');
  assert(timestamp[13] == ':' && timestamp[16] == ':');

  std::cout << "TestGetTimestamp() was successful!" << std::endl;
}

/**
 * @brief Status gets converted into a string
 * @param level that will be converted
 * @return Current status as a string in Upper-Case
 */
std::string Logging::LevelToString(LogLevel level) const {
  switch (level) {
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
  }
}

/**
 * @brief Current level as a string
 * @return Status as a string
 */
std::string Logging::GetLevel() {
  std::lock_guard<std::mutex> lock(logMutex_);
  return LevelToString(currentLevel_);
}

/**
 * @brief Write status-data in to the log-file
 * @param message Message that will be displayed
 * @param level Current LogLevel
 */
void Logging::Log(const std::string& message, LogLevel level) {
  if (level < currentLevel_)
    return;

  std::string timestamp = GetTimestamp();
  std::string levelStr = LevelToString(level);
  std::ostringstream logEntry;
  logEntry << "[" << timestamp << "] [" << levelStr << "] " << message << "\n";

  std::lock_guard<std::mutex> lock(logMutex_);
  if (logFile_.is_open()) {
    logFile_ << logEntry.str();
    logFile_.flush();
  }
  if (consoleOutput_) {
    std::cout << logEntry.str();
  }
}

}  // namespace observer::logging
