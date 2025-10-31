#include <iostream>

#include "utils/gpu.hpp"
#include "utils/monitoring.hpp"

int main() {
  // std::cout << "CPU in idle state: " << observer::cpu::GetIdlePercentage() << std::endl;
  // std::cout << "Switches: " << observer::cpu::GetContextSwitchesPerSec() << std::endl;
  // std::cout << "Interrupts per second: " << observer::cpu::GetInterruptsPerSec() << std::endl;
  // observer::monitoring::ShowRealTimeAsciiUIForCPU();
  observer::gpu::ShowGPUInfo();
  return 0;
}
