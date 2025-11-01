#ifndef INC_UTILS_GPU_HPP_
#define INC_UTILS_GPU_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "utils/utilities.hpp"

namespace observer::gpu {

enum class GPUCompany { NVIDIA, AMD, INTEL, UNKNOWN };

struct GPUInfo {
  std::string name;
  double temperature = 0.0;
  double usage = 0.0;
  double memory_used = 0.0;
  double memory_total = 0.0;
  double clock_frequency = 0.0;
  GPUCompany company = GPUCompany::UNKNOWN;
};

GPUInfo ReadGPUInfo();

void ShowGPUInfo();

GPUCompany DetectGPUCompany();

const std::string GetVerboseGPUInfo();

}  // namespace observer::gpu

#endif /* INC_UTILS_GPU_HPP_ */
