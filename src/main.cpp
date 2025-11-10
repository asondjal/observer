#include <iostream>

#include "utils/monitoring.hpp"
#include "utils/storage.hpp"

int main() {
  // std::cout << "CPU in idle state: " << observer::cpu::GetIdlePercentage() << std::endl;
  // std::cout << "Switches: " << observer::cpu::GetContextSwitchesPerSec() << std::endl;
  // std::cout << "Interrupts per second: " << observer::cpu::GetInterruptsPerSec() << std::endl;
  // observer::monitoring::ShowRealTimeAsciiUIForCPU();
  // observer::storage::SaveConfidentialStorageData();
  observer::storage::DisplayData();
  return 0;
}
