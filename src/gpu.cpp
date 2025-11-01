#include "utils/gpu.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace observer::gpu {

GPUCompany DetectGPUCompany() {
  // NVIDIA-GPU
  if (std::system("which nvidia-smi > /dev/null 2>&1") == 0)
    return GPUCompany::NVIDIA;

  // AMD-GPU
  if (fs::exists("/sys/class/drm/card0/device/driver/module") &&
      fs::read_symlink("/sys/class/drm/card0/device/driver/module").string().find("amdgpu") !=
          std::string::npos)
    return GPUCompany::AMD;

  // INTEL-GPU
  if (fs::exists("/sys/class/drm/card0/device/") &&
      fs::exists("/sys/class/drm/card0/device/vendor")) {
    std::ifstream vendor_file("/sys/class/drm/card0/device/vendor");
    std::string vendor_id;
    vendor_file >> vendor_id;
    if (vendor_id == "0x8086")  // ID for Intel
      return GPUCompany::INTEL;
  }

  return GPUCompany::UNKNOWN;
}

GPUInfo ReadGPUInfo() {
  GPUInfo gpu_info;
  gpu_info.company = DetectGPUCompany();

  switch (gpu_info.company) {
    case GPUCompany::NVIDIA: {
      // Query over nvidia-smi
      std::string cmd =
          "nvidia-smi --query-gpu=name,temperature.gpu,utilization.gpu,"
          "memory.used,memory.total,clocks.gr --format=csv,noheader,nounits > /tmp/gpu_info.txt";
      std::system(cmd.c_str());

      std::ifstream file("/tmp/gpu_info.txt");
      std::getline(file, gpu_info.name, ',');
      file >> gpu_info.temperature >> gpu_info.usage >> gpu_info.memory_used >>
          gpu_info.memory_total >> gpu_info.clock_frequency;
      std::remove("/tmp/gpu_info.txt");
      break;
    }

    case GPUCompany::AMD: {
      gpu_info.name = "AMD GPU (detected)";
      try {
        std::ifstream temp_file("/sys/class/drm/card0/device/hwmon/hwmon0/temp1_input");
        int millideg;
        if (temp_file >> millideg)
          gpu_info.temperature = millideg / 1000.0;

        std::ifstream usage_file("/sys/class/drm/card0/device/gpu_busy_percent");
        usage_file >> gpu_info.usage;

        std::ifstream mem_used("/sys/class/drm/card0/device/mem_info_vram_used");
        std::ifstream mem_total("/sys/class/drm/card0/device/mem_info_vram_total");
        unsigned long used, total;
        mem_used >> used;
        mem_total >> total;
        gpu_info.memory_used = used / (1024.0 * 1024.0);
        gpu_info.memory_total = total / (1024.0 * 1024.0);
      } catch (...) {
        std::cerr << "Error reading AMD GPU data.\n";
      }
      break;
    }

    case GPUCompany::INTEL: {
      gpu_info.name = "Intel Integrated GPU";
      gpu_info.temperature = 45.0;  // Placeholder (Intel-Interface ist quite complex)
      gpu_info.usage = 15.0;
      break;
    }

    case GPUCompany::UNKNOWN:
    default: {
      gpu_info.name = "Unknown GPU";
      gpu_info.temperature = 0.0;
      gpu_info.usage = 0.0;
      break;
    }
  }

  return gpu_info;
}

const std::string GetVerboseGPUInfo() {
  GPUInfo info = ReadGPUInfo();
  std::stringstream ss;

  ss << "=== GPU INFORMATION ===\n"
     << "Name:          " << info.name << "\n"
     << "Temperature:   " << info.temperature << " °C\n"
     << "Usage:         " << info.usage << " %\n"
     << "Memory Used:   " << info.memory_used << " MB\n"
     << "Memory Total:  " << info.memory_total << " MB\n"
     << "Clock:         " << info.clock_frequency << " GHz\n";

  std::ofstream outfile("./confidential/gpu_info.txt");
  outfile << ss.str();
  outfile.close();
  observer::utilities::SecureFileOwnership("./confidential/gpu_info.txt");

  return ss.str();
}

/**
 * @brief Display of the attributes of the GPU
 */
void ShowGPUInfo() {
  GPUInfo info = ReadGPUInfo();
  std::cout << GetVerboseGPUInfo();
}

}  // namespace observer::gpu
