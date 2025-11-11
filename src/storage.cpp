#include "utils/storage.hpp"

namespace fs = std::filesystem;

namespace observer::storage {

// Minimum size for storage devices (in GB)
constexpr double MIN_STORAGE_GB = 16.00;

/**
 * @brief Read the storage model
 */
const std::string GetStorageModel(const std::string& device) {
  std::string model_path = "/sys/block/" + device + "/device/model";
  std::ifstream file(model_path);
  std::string model;
  if (file.is_open()) {
    std::getline(file, model);
  } else {
    model = "Unknown Model";
  }
  return model;
}

/**
 * @brief Detect the type of the storage device (NVMe, SSD, HDD, USB, ...).
 * @return Type of the storage device
 */
const std::string GetStorageType(const std::string& device) {
  if (device.find("nvme") == 0)
    return "NVMe";
  if (device.find("sd") == 0)
    return "SATA/SSD";
  if (device.find("mmcblk") == 0)
    return "eMMC";
  if (device.find("sr") == 0)
    return "Optical Drive";
  return "Unknown";
}

/**
 * @brief Detect the storage size of a physical drive.
 * @return Storage size of the device
 */
double GetStorageSizeGB(const std::string& device) {
  std::cout << std::fixed << std::setprecision(2);
  std::ifstream size_file("/sys/block/" + device + "/size");
  unsigned long long sectors = 0;
  size_file >> sectors;

  // Each section contains data of probably 512 byte
  double bytes = sectors * 512.0;
  return bytes / (1024.0 * 1024.0 * 1024.0);
}

/**
 * @brief Read data from the mountpoint.
 */
std::vector<double> GetStorageUsage(const std::string& mountpoint) {
  std::vector<double> result(3, 0.0);
  if (mountpoint.empty())
    return result;

  struct statvfs stats;
  if (statvfs(mountpoint.c_str(), &stats) == 0) {
    double total =
        static_cast<double>(stats.f_blocks) * stats.f_frsize / (1024.0 * 1024.0 * 1024.0);
    double free = static_cast<double>(stats.f_bfree) * stats.f_frsize / (1024.0 * 1024.0 * 1024.0);
    double used = total - free;
    result = {used, free, total};
  }
  return result;
}

/**
 * @brief Read mountpoints including their corresponding devices.
 */
std::map<std::string, std::string> GetMountpoints() {
  std::map<std::string, std::string> mounts;
  std::ifstream mounts_file("/proc/mounts");
  std::string device, mountpoint, rest;
  while (mounts_file >> device >> mountpoint) {
    std::getline(mounts_file, rest);
    mounts[device] = mountpoint;
  }
  return mounts;
}

/**
 * @brief Read all existing drive that contain atleast 16 GB of storage
 */
std::vector<StorageInfo> ReadAllStorageDevices() {
  std::vector<StorageInfo> storages;
  auto mountpoints = GetMountpoints();

  for (const auto& entry : fs::directory_iterator("/sys/block")) {
    std::string device = entry.path().filename();
    double total_gb = GetStorageSizeGB(device);

    if (total_gb < MIN_STORAGE_GB)
      continue;

    StorageInfo info;
    info.device_name = device;
    info.model = GetStorageModel(device);
    info.type = GetStorageType(device);
    info.total_GB = total_gb;

    // Search for the partitions with the biggest storage
    std::string dev_path_prefix = "/dev/" + device;
    std::string best_mountpoint;
    double best_used = 0.0;
    double best_total = 0.0;

    for (const auto& [mounted_device, mountpoint] : mountpoints) {
      if (mounted_device.find(dev_path_prefix) == 0) {
        auto usage = GetStorageUsage(mountpoint);
        double total = usage[2];
        double used = usage[0];

        // Take partition containing the biggest storage
        if (total > best_total) {
          best_total = total;
          best_used = used;
          best_mountpoint = mountpoint;
        }
      }
    }

    if (!best_mountpoint.empty()) {
      info.used_GB = best_used;
      info.free_GB = best_total - best_used;
      info.used_percent = (best_total > 0) ? (100.0 * best_used / best_total) : 0.0;
    } else {
      info.used_GB = 0;
      info.free_GB = total_gb;
      info.used_percent = 0.0;
    }

    storages.push_back(info);
  }

  return storages;
}

const std::string GetVerboseStorageInfo() {
  return observer::utilities::GetVerboseInfo("/proc/mounts");
}

void SaveConfidentialStorageInfo() {
  std::string document = "/storage_info.txt";
  std::string file = observer::utilities::SaveConfidentialData(document);

  std::ofstream outfile(file, std::ios::trunc);
  outfile << GetVerboseStorageInfo();
  outfile.close();

  observer::utilities::SecureFileOwnership(file);
}

/**
 * @brief Display of all registered storage devices
 */
void ShowAllStorageDevices() {
  std::vector<StorageInfo> storages = ReadAllStorageDevices();

  std::cout << "=== STORAGE DEVICES (>= 16 GB) ===\n";
  for (const auto& s : storages) {
    std::cout << "Device:  " << s.device_name << "\n"
              << "Model:   " << s.model << "\n"
              << "Type:    " << s.type << "\n"
              << "Total:   " << s.total_GB << " GB\n"
              << "Used:    " << s.used_GB << " GB (" << s.used_percent << "%)\n"
              << "Free:    " << s.free_GB << " GB\n"
              << "------------------------------\n";
  }
}

}  // namespace observer::storage
