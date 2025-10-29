#ifndef INC_UTILS_CPP_HPP_
#define INC_UTILS_CPP_HPP_

#include <chrono>
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "utils/utilities.hpp"

namespace observer::cpu {
struct CPUUsage {
  double total = 0.0;
  double idle = 0.0;
};

std::string const GetVerboseCPUInfo();
std::string const GetCPUInfo();
void SaveConfidentialCPUInfo();
std::vector<double> const GetAllCPUTemperatures();
double GetAverageCPUTemperature();
void ShowAllCPUTemperatures();
std::vector<double> const GetAllCPUFrequencies();
void ShowAllCPUFrequencies();
double GetAverageCPUFrequency();
std::vector<CPUUsage> ReadCPUStats();
std::vector<double> GetCPULoadPerCore();
void ShowAllLoadsPerCPU();
}  // namespace observer::cpu

#endif /* INC_UTILS_CPP_HPP_ */
