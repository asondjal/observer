#include <iostream>

#include "utils/cpu.hpp"
#include "utils/ui_ascii.hpp"

int main() {
  std::cout << "Choose Mode:\n";
  std::cout << "[1] BASIC" << " or " << "[2] REAL-TIME" << std::endl;
  int choice;
  std::cin >> choice;

  if (choice == 1)
    observer::ShowAsciiUI(false);
  else if (choice == 2)
    observer::ShowAsciiUI(true);
}
