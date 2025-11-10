#include "utils/storage.hpp"

namespace fs = std::filesystem;

namespace observer::storage {

const std::string GetStorageDeviceName() {
  for (const auto& entry : fs::directory_iterator("/sys/block")) {
    std::string dev = entry.path().filename().string();
    // Only physical drives
    if (dev.find("loop") == std::string::npos && dev.find("ram") == std::string::npos)
      return dev;
  }
  return "unknown";
}

const std::string GetStorageModel() {
    const std::string device_name = GetStorageDeviceName();
  std::string path = "/sys/block/" + device_name + "/device/model";
  std::ifstream model_file(path);
  if (!model_file.is_open())
    return "unknown";

  std::string model;
  std::getline(model_file, model);
  return model.empty() ? "unknown" : model;
}

const std::string GetStorageType() {
        const std::string device_name = GetStorageDeviceName();
  if (device_name.find("nvme") == 0)
    return "NVMe";

  std::string path = "/sys/block/" + device_name + "/queue/rotational";
  std::ifstream file(path);
  if (!file.is_open())
    return "unknown";

  int rotational = 0;
  file >> rotational;
  return (rotational == 0) ? "SSD" : "HDD";
}

std::vector<double> GetStorageUsage() {
  std::vector<double> values(3, 0.0);  // {used, free, total}

  FILE* pipe = popen("df --output=used,avail -BG / | tail -1", "r");
  if (pipe) {
    unsigned long used = 0, avail = 0;
    fscanf(pipe, "%luG %luG", &used, &avail);
    pclose(pipe);

    double total = used + avail;
    values[0] = used;
    values[1] = avail;
    values[2] = total;
  }

  return values;
}

StorageInfo ReadStorageInfo() {
  StorageInfo info;
  info.device_name = GetStorageDeviceName();
  info.model = GetStorageModel();
  info.type = GetStorageType();

  const auto usage = GetStorageUsage();
  info.used_GB = usage[0];
  info.free_GB = usage[1];
  info.total_GB = usage[2];
  info.used_percent = (info.total_GB > 0) ? 100.0 * info.used_GB / info.total_GB : 0.0;

  return info;
}

const std::string GetVerboseStorageInfo() {
  return observer::utilities::GetVerboseInfo("/sys/block");
}

void SaveConfidentialStorageData() {
  std::string storage_info = "/storage_info.txt";
  std::string file = observer::utilities::SaveConfidentialData(storage_info);

  std::ofstream outfile(file, std::ios::trunc);
  outfile << GetVerboseStorageInfo();
  outfile.close();

  observer::utilities::SecureFileOwnership(file);
}

void DisplayData() {
    StorageInfo basics = ReadStorageInfo();
    std::cout << "Device name: " << basics.device_name << std::endl;
    std::cout << "Device model: " << basics.model << std::endl;
    std::cout << "Device type: " << basics.type << std::endl;
    std::cout << "Total storage: " << basics.total_GB << " GB" << std::endl;
    std::cout << "Free storage: " << basics.free_GB << " GB" << std::endl;
    std::cout << "Used storage: " << basics.used_GB << " GB" << std::endl;
    std::cout << "Used percentage: " << basics.used_percent << " " << std::endl;
}



}
