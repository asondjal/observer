#include "utils/utilities.hpp"

namespace observer::utilities {
/**
 * @brief Management of access and ownership rights towards confidential data
 * @param file File whose access rights should be limited
 */
void SecureFileOwnership(const std::string& file) {
  if (chmod(file.c_str(), S_IRUSR | S_IWUSR) != 0) {
    std::string error_message = "chmod failed: insufficient permissions";
    observer::utilities::GetLogger().Log(error_message, observer::logging::LogLevel::ERROR);
  } else {
    observer::utilities::GetLogger().Log("File permissions set to 600",
                                         observer::logging::LogLevel::INFO);
  }
}

/**
 * @brief Reading data from a file and storing in a buffer
 * @param file document that is supposed to be read
 * @return concatenated file content
 */
std::string GetVerboseInfo(const std::string& file) {
  std::ifstream info(file);
  std::stringstream buffer;
  buffer << info.rdbuf();
  return buffer.str();
}

/**
 * @brief Saving confidential info
 * @param document associated file whose content should be saved into a confidential folder
 * @return path to the file
 */
std::string SaveConfidentialData(std::string& document) {
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
}  // namespace observer::utilities
