#include "utils/app.hpp"
#include "utils/monitoring.hpp"

int main() {
  observer::app::RunObserverApp();
  // observer::monitoring::ShowRealTimeMinimumAsciiUIForCPU();
  // observer::monitoring::ShowRealTimeMinimumAsciiUIForRAM();
  return 0;
}
