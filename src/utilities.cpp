#include "utils/utilities.hpp"

namespace observer::utilities {

/**
 * @brief Reading data from a file and storing in a buffer
 * @param file document that is supposed to be read
 * @return concatenated file content
 */
const std::string GetVerboseInfo(const std::string& file) {
  observer::read_data::ReadData content(file);
  return content.ReadFile();
}

/**
 * @brief Detect the current user
 * @return Name of the current user
 */
const std::string GetCurrentUser() {
  const char* username = getlogin();
  if (username)
    return std::string(username);

  struct passwd* pw = getpwuid(getuid());
  return pw ? std::string(pw->pw_name) : "unknown";
}

/**
 * @brief Detect the file owner
 * @param path Path towards the file
 * @return Name of the file owner
 */
const std::string GetFileOwner(const std::string& path) {
  struct stat info;
  if (stat(path.c_str(), &info) != 0)
    return "unknown";

  struct passwd* pw = getpwuid(info.st_uid);
  return pw ? std::string(pw->pw_name) : "unknown";
}

/**
 * @brief Management of access and ownership rights towards confidential data
 * @param file File whose access rights should be limited
 */
void SecureFileOwnership(const std::string& file) {
  try {
    namespace fs = std::filesystem;

    std::string current_user = GetCurrentUser();
    std::string file_owner = GetFileOwner(file);

    if (current_user == "unknown" || file_owner == "unknown") {
      observer::utilities::GetLogger().Log(
          "SecureFileOwnership: Could not determine user or owner for " + file,
          observer::logging::LogLevel::ERROR);
      return;
    }

    if (current_user != file_owner) {
      observer::utilities::GetLogger().Log("File ownership mismatch detected! File: " + file +
                                               " | Owner: " + file_owner +
                                               " | Current user: " + current_user,
                                           observer::logging::LogLevel::WARNING);

    } else {
      observer::utilities::GetLogger().Log(
          "SecureFileOwnership: File owner verified -> " + file_owner,
          observer::logging::LogLevel::INFO);
    }

  } catch (const std::exception& e) {
    observer::utilities::GetLogger().Log(
        "SecureFileOwnership: Exception occurred -> " + std::string(e.what()),
        observer::logging::LogLevel::ERROR);
  }
}

/**
 * @brief Saving confidential info
 * @param document associated file whose content should be saved into a confidential folder
 * @return path to the saved file containing the confidential data
 */
const std::string SaveConfidentialData(std::string& document) {
  namespace fs = std::filesystem;
  std::string dir = "confidential";
  std::string file = dir + document;
  observer::utilities::GetLogger().Log("Data was saved to: " + file,
                                       observer::logging::LogLevel::INFO);

  if (!fs::exists(dir)) {
    fs::create_directory(dir);
  }
  return file;
}

/**
 * @brief Current Time of the operating system
 * @return Time in format `TT:MM:YY H:M:S`
 */
const std::string GetSystemTimestamp() {
  auto now = std::chrono::system_clock::now();
  auto itt = std::chrono::system_clock::to_time_t(now);
  std::ostringstream ss;
  ss << std::put_time(std::localtime(&itt), "%d.%m.%Y %H:%M:%S");
  return ss.str();
}
}  // namespace observer::utilities
