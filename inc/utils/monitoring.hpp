#ifndef INC_UTILS_MONITORING_HPP_
#define INC_UTILS_MONITORING_HPP_

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>

#include "utils/cpu.hpp"
#include "utils/ram.hpp"
#include "utils/storage.hpp"

namespace observer::monitoring {
void ShowInitialData();
void GetUserChoiceFromInitialDataUI(int user_choice);
// ftxui::Component ObserverMonitorComponent();

void ShowRealTimeAsciiUIForCPU();
void ShowRealtTimeAsciiUIForRAM();
void ShowRealTimeAsciiUIForStorage();
}  // namespace observer::monitoring

#endif /* INC_UTILS_MONITORING_HPP_ */