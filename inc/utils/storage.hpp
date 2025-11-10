#ifndef INC_UTILS_STORAGE_HPP_
#define INC_UTILS_STORAGE_HPP_

#include <iostream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <sstream>

#include "utils/utilities.hpp"

namespace observer::storage {
struct StorageInfo {
  std::string device_name;     // z. B. "nvme0n1", "sda"
  std::string model;           // z. B. "Samsung SSD 990 PRO"
  std::string type;            // "NVMe", "SSD", "HDD"
  double total_GB = 0.0;
  double used_GB = 0.0;
  double free_GB = 0.0;
  double used_percent = 0.0;
};

const std::string GetStorageDeviceName();
const std::string GetStorageModel();
const std::string GetStorageType();
std::vector<double> GetStorageUsage();
StorageInfo ReadStorageInfo();
void SaveConfidentialStorageData();
const std::string GetVerboseStorageInfo();
void DisplayData();
}

#endif /* INC_UTILS_STORAGE_HPP_ */

