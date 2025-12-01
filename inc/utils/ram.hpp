#ifndef INC_UTILS_RAM_HPP_
#define INC_UTILS_RAM_HPP_

#include <iomanip>

#include "utils/utilities.hpp"

namespace observer::ram {

struct RAMInfo {
  double total_MB = 0;
  double available_MB = 0;
  double used_MB = 0;
  double used_percent = 0;
};

RAMInfo ReadRAMInfo();

const std::string GetVerboseRAMInfo();

void SaveConfidentialRAMInfo();

void DisplayData();
}  // namespace observer::ram

#endif /* INC_UTILS_RAM_HPP_ */
