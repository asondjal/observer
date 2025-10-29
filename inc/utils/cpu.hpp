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

#include "utils/logging.hpp"
#include "utils/utilities.hpp"

namespace observer::cpu {

std::string const GetVerboseCPUInfo();
std::string const GetCPUInfo();
void SaveConfidentialCPUInfo();
std::vector<double> const GetAllCPUTemperatures();
double GetAverageCPUTemperature();
void ShowAllCPUTemperatures();
std::vector<double> const GetAllCPUFrequencies();
void ShowAllCPUFrequencies();
double GetAverageCPUFrequency();
}  // namespace observer::cpu

#endif /* INC_UTILS_CPP_HPP_ */
