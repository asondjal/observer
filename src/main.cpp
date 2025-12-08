#include "utils/app.hpp"
#include "utils/monitoring.hpp"

int main() {
  observer::app::RunObserverApp();
  // observer::monitoring::ShowRealTimeMinimumAsciiUIForCPU();
  return 0;
}
