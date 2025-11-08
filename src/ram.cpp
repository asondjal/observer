#include "utils/ram.hpp"

namespace observer::ram {

const std::string GetVerboseRAMInfo() {
  return observer::utilities::GetVerboseInfo("/proc/meminfo");
}

RAMInfo ReadRAMInfo() {
  RAMInfo info;
  std::ifstream meminfo("/proc/meminfo");
  std::string key;
  unsigned long value;
  std::string unit;

  unsigned long total = 0, available = 0;
  while (meminfo >> key >> value >> unit) {
    if (key == "MemTotal:")
      total = value;
    if (key == "MemAvailable:")
      available = value;
  }

  info.total_MB = total / 1024.0;
  info.available_MB = available / 1024.0;
  info.free_MB = info.available_MB;
  info.used_percent = 100.0 * (1.0 - info.available_MB / info.total_MB);

  return info;
}

void SaveConfidentialRAMInfo() {
  std::string ram_info = "/ram_info.txt";
  std::string file = observer::utilities::SaveConfidentialData(ram_info);

  std::ofstream outfile(file, std::ios::trunc);
  outfile << GetVerboseRAMInfo();
  outfile.close();

  observer::utilities::SecureFileOwnership(file);
}

void DisplayData() {
  const std::string path = "./confidential/ram_info.txt";
  std::ifstream infile(path);
  if (!infile) {
    std::cerr << "Failed to open " << path << '\n';
    return;
  }

  std::string line;
  for (int i = 0; i < 12 && std::getline(infile, line); i++) {
    std::cout << line << '\n';
  }
}

}  // namespace observer::ram