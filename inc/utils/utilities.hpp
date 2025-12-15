#ifndef INC_UTILS_UTILITIES_HPP_
#define INC_UTILS_UTILITIES_HPP_

#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "utils/logging.hpp"
#include "utils/read_data.hpp"

namespace observer::utilities {
inline observer::logging::Logging& GetLogger() {
  static observer::logging::Logging logger("./confidential/monitoring_logs.txt",
                                           observer::logging::LogLevel::INFO);
  return logger;
}

const std::string GetVerboseInfo(const std::string& file);
const std::string SaveConfidentialData(std::string& document);
const std::string GetSystemTimestamp();
const std::string GetCurrentUser();
const std::string GetFileOwner(const std::string& path);
void SecureFileOwnership(const std::string& file);
/**
 * @brief Formats a double value with specified decimal precision
 * @param value The double value to format
 * @param precision Number of decimal places (default: 2)
 * @return Formatted string with fixed precision
 */
inline std::string FormatDouble(double value, int precision = 2) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(precision) << value;
  return oss.str();
}

/**
 * @brief Formats a double as an integer (0 decimals)
 * @param value The double value to format
 * @return Formatted string with no decimal places
 */
inline std::string FormatAsInt(double value) {
  return FormatDouble(value, 0);
}
}  // namespace observer::utilities

#endif /* INC_UTILS_UTILITIES_HPP_ */
