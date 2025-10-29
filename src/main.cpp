#include <iostream>

#include "utils/monitoring.hpp"

int main() {
  // std::cout << "Choose Mode:\n";
  // std::cout << "[1] BASIC" << " or " << "[2] REAL-TIME" << std::endl;
  // int choice;
  // std::cin >> choice;

  // if (choice == 1)
  //   observer::monitoring::ShowInitialData();
  // else if (choice == 2)
  //   observer::monitoring::ShowRealTimeAsciiUIForCPU();
  observer::cpu::ShowAllLoadsPerCPU();
  return 0;
}
