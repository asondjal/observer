#ifndef INC_UTILS_MONITORING_HPP_
#define INC_UTILS_MONITORING_HPP_

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

#include "utils/cpu.hpp"
#include "utils/graph.hpp"
#include "utils/ram.hpp"
#include "utils/storage.hpp"

namespace observer::monitoring {
void ShowRealTimeMinimumAsciiUIForCPU();
void ShowRealTimeMaximumAsciiUIForCPU();
void ShowRealTimeMinimumAsciiUIForRAM();
void ShowRealtTimeMaximumAsciiUIForRAM();
void ShowRealTimeMaximumAsciiUIForStorage();
void ShowRealTimeMinimumAsciiUIForStorage();
int OptionDetectionForStartingMenu();
}  // namespace observer::monitoring

#endif /* INC_UTILS_MONITORING_HPP_ */