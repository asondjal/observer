#ifndef INC_UTILS_STORAGE_HPP_
#define INC_UTILS_STORAGE_HPP_

#include <sys/statvfs.h>

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "utils/utilities.hpp"

namespace observer::storage {
struct StorageInfo {
  std::string device_name;
  std::string model;
  std::string type;
  double total_GB = 0.0;
  double used_GB = 0.0;
  double free_GB = 0.0;
  double used_percent = 0.0;
};

const std::string GetStorageModel(const std::string& device);
const std::string GetStorageType(const std::string& device);
double GetStorageSizeGB(const std::string& device);
std::vector<double> GetStorageUsage(const std::string& mountpoint);
std::map<std::string, std::string> GetMountpoints();
std::vector<StorageInfo> ReadAllStorageDevices();
void ShowAllStorageDevices();
const std::string GetVerboseStorageInfo();
void SaveConfidentialStorageInfo();
}  // namespace observer::storage

#endif /* INC_UTILS_STORAGE_HPP_ */
