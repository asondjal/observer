#include <iostream>

#include "utils/monitoring.hpp"

using namespace ftxui;

int main() {
  // std::cout << "CPU in idle state: " << observer::cpu::GetIdlePercentage() << std::endl;
  // std::cout << "Switches: " << observer::cpu::GetContextSwitchesPerSec() << std::endl;
  // std::cout << "Interrupts per second: " << observer::cpu::GetInterruptsPerSec() << std::endl;
  // observer::monitoring::ShowInitialData();
  // observer::monitoring::ShowRealTimeAsciiUIForCPU();
  // observer::storage::SaveConfidentialStorageData();
  // observer::storage::SaveConfidentialStorageInfo();
  // observer::storage::GetVerboseStorageInfo();
  // observer::storage::ShowAllStorageDevices();
  // observer::monitoring::GetUserChoiceFromInitialDataUI(1);
  // observer::monitoring::GetUserChoiceFromInitialDataUI(2);
  // observer::monitoring::GetUserChoiceFromInitialDataUI(3);
  // observer::monitoring::GetUserChoiceFromInitialDataUI(0);
  observer::monitoring::ShowRealTimeAsciiUIForStorage();

  return 0;
}
