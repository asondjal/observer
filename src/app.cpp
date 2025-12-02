#include "utils/app.hpp"

namespace observer::app {

/**
 * @brief Runs the main application loop for the Observer system monitor.
 */
void RunObserverApp() { observer::monitoring::OptionDetectionForStartingMenu(); }

/**
 * @brief Gets user choice from the initial data UI and triggers corresponding monitoring UI.
 * @param user_choice user's choice represented as an integer.
 */
void GetUserChoiceFromInitialDataUI(int user_choice) {
  switch (user_choice) {
    case 0:
    
    case 1:
      observer::monitoring::ShowRealTimeMaximumAsciiUIForCPU();
      break;
    case 2:
      observer::monitoring::ShowRealTimeMaximumAsciiUIForStorage();
      break;
    case 3:
      observer::monitoring::ShowRealtTimeMaximumAsciiUIForRAM();
      break;
    default:
      std::cout << "User didn't select an option!" << std::endl;
  }
}

/**
 * @brief Sets up the menu for the Observer application.
 */
void Launch() {
  int choice = observer::monitoring::OptionDetectionForStartingMenu();
  GetUserChoiceFromInitialDataUI(choice);
}

}  // namespace observer::app