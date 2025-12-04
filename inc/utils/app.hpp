#ifndef INC_UTILS_APP_HPP_
#define INC_UTILS_APP_HPP_

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "utils/cpu.hpp"
#include "utils/monitoring.hpp"
#include "utils/ram.hpp"
#include "utils/storage.hpp"

namespace observer::app {

void RunObserverApp();
}  // namespace observer::app

#endif /* INC_UTILS_APP_HPP_ */