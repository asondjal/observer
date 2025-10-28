#include <iostream>

#include "utils/cpu.hpp"

int main() {
  std::cout << "=== MONITORING INFORMATION ===\n";
  observer::cpu::ShowAllCPUTemperatures();
  std::cout << observer::cpu::GetAverageCPUTemperature() << std::endl;
  return 0;
}
