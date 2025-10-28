#ifndef INC_UTILS_UTILITIES_HPP_
#define INC_UTILS_UTILITIES_HPP_

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

void SecureFileOwnership(const std::string& file);

std::string GetVerboseInfo(const std::string& file);

std::string SaveConfidentialData(std::string& document);
}  // namespace observer::utilities

#endif /* INC_UTILS_UTILITIES_HPP_ */
