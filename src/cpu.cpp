#include "utils/cpu.hpp"

namespace observer::cpu {

/**
 * @brief Verbose information about the CPU
 * @return Verbose information
 */
const std::string GetVerboseCPUInfo() {
  return observer::utilities::GetVerboseInfo("/proc/cpuinfo");
}

/**
 * @brief Basic information from the CPU
 * @return Model Name
 */
const std::string GetCPUModel() {
  std::ifstream cpuinfo("/proc/cpuinfo");
  std::string line;
  while (std::getline(cpuinfo, line)) {
    if (line.find("model name") != std::string::npos)
      return line.substr(line.find(":") + 2);
  }
  return "Unknown CPU";
}

/**
 * @brief Saving confidential cpu info
 */
void SaveConfidentialCPUInfo() {
  std::string document = "/cpu_info.txt";
  std::string file = observer::utilities::SaveConfidentialData(document);

  std::ofstream outfile(file, std::ios::trunc);
  outfile << GetVerboseCPUInfo();
  outfile.close();

  observer::utilities::SecureFileOwnership(file);
}

/**
 * @brief Temperature measurement of the CPU-Threads
 * @return Temperatures of the CPU-Threads
 */
const std::vector<double> GetAllCPUTemperatures() {
  namespace fs = std::filesystem;
  std::vector<double> temperatures;

  try {
    for (const auto& hwmon : fs::directory_iterator("/sys/class/hwmon")) {
      for (const auto& file : fs::directory_iterator(hwmon.path())) {
        const std::string filename = file.path().filename().string();

        if (filename.find("temp") != std::string::npos &&
            filename.find("_input") != std::string::npos) {
          std::ifstream temp_file(file.path());
          double temp_millideg;
          if (temp_file >> temp_millideg) {
            std::cout << std::fixed << std::setprecision(2);
            temperatures.push_back(temp_millideg / 1000.0);
          }
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error occurred while reading the document: " << e.what() << std::endl;
  }

  return temperatures;
}
/**
 * @brief Display of all temperatures for the threads on the CPU
 * required for debugging
 */
void ShowAllCPUTemperatures() {
  std::vector<double> values = observer::cpu::GetAllCPUTemperatures();
  for (std::size_t i = 0; i < values.size(); i++) {
    std::cout << i << ". Sensor: " << values[i] << " °C" << std::endl;
  }
}

/**
 * @brief Calculation of the average temperature for the CPU
 * @return Average temperature for the CPU with an accuracy of 2 decimals
 */
double GetAverageCPUTemperature() {
  std::cout << std::fixed << std::setprecision(2);
  std::vector<double> cpu_temps = GetAllCPUTemperatures();
  double average_cpu_temp = 0.0000;
  for (double value : cpu_temps) {
    average_cpu_temp += value;
  }
  return (average_cpu_temp / (cpu_temps.size()));
}

/**
 * @brief Read current frequencies of the CPU-cores
 * @return Current frequencies saved in a std::vector
 */
const std::vector<double> GetAllCPUFrequencies() {
  namespace fs = std::filesystem;
  std::vector<double> frequencies;

  try {
    for (const auto& cpu_dir : fs::directory_iterator("/sys/devices/system/cpu")) {
      const std::string cpu_path = cpu_dir.path().string();
      if (cpu_path.find("cpu") != std::string::npos &&
          cpu_path.find("cpuidle") == std::string::npos &&
          cpu_path.find("cpufreq") == std::string::npos) {
        std::string freq_file = cpu_path + "/cpufreq/scaling_cur_freq";
        if (fs::exists(freq_file)) {
          std::ifstream freq_stream(freq_file);
          double freq_khz = 0.0;
          if (freq_stream >> freq_khz) {
            frequencies.push_back(freq_khz / 1000.0);
          }
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Error while reading CPU frequencies: " << e.what() << std::endl;
  }

  return frequencies;
}
/**
 * @brief Display of current frequencies of the CPU-cores for debugging
 */
void ShowAllCPUFrequencies() {
  std::vector<double> freqs = GetAllCPUFrequencies();
  std::cout << std::fixed << std::setprecision(2);
  for (std::size_t i = 0; i < freqs.size(); i++) {
    std::cout << "Core " << i << ": " << freqs[i] << " MHz" << std::endl;
  }
}

/**
 * @brief Calculation of the average frequency altogether
 * @return Average frequency with an accuracy of 2 decimals
 */
double GetAverageCPUFrequency() {
  std::cout << std::fixed << std::setprecision(2);
  std::vector<double> freqs = GetAllCPUFrequencies();
  if (freqs.empty())
    return 0.0;
  double sum = 0.0;
  for (double f : freqs) {
    sum += f;
  }
  return sum / freqs.size();
}

/**
 * @brief Complex function that reads the absolute load for each CPU-core
 * @return std::vector<CPUUsage> containing the absolute load for each CPU-core
 */
std::vector<CPUUsage> ReadCPUStats() {
  std::cout << std::fixed << std::setprecision(2);
  std::ifstream file("/proc/stat");
  std::string line;
  std::vector<CPUUsage> stats;

  while (std::getline(file, line)) {
    if (line.rfind("cpu", 0) != 0)
      break;
    if (line.substr(0, 3) == "cpu ")
      continue;

    std::istringstream iss(line);
    std::string cpu_label;
    CPUUsage usage;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    iss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    usage.idle = idle + iowait;
    usage.total = user + nice + system + idle + iowait + irq + softirq + steal;

    stats.push_back(usage);
  }
  return stats;
}

/**
 * @brief Calculation of the relative CPU-Load per core (in%)
 * @return std::vector<double> containing the relative load for each CPU-core
 */
std::vector<double> GetCPULoadPerCore() {
  std::cout << std::fixed << std::setprecision(2);
  std::vector<CPUUsage> prev = ReadCPUStats();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::vector<CPUUsage> curr = ReadCPUStats();

  std::vector<double> usage;
  usage.reserve(curr.size());

  for (size_t i = 0; i < curr.size(); ++i) {
    double total_diff = curr[i].total - prev[i].total;
    double idle_diff = curr[i].idle - prev[i].idle;
    double load = 100.0 * (total_diff - idle_diff) / total_diff;
    usage.push_back(load);
  }
  return usage;
}

/**
 * @brief Display of current relative load per CPU-core
 */
void ShowAllLoadsPerCPU() {
  std::cout << std::fixed << std::setprecision(2);
  std::vector<double> load_management = GetCPULoadPerCore();
  for (size_t i = 0; i < load_management.size(); i++) {
    std::cout << i << ". Core: " << load_management[i] << " %" << std::endl;
  }
}

/**
 * @brief Reads the overall CPU idle percentage.
 * @return the system-wide CPU idle percentage
 */
double GetIdlePercentage() {
  std::cout << std::fixed << std::setprecision(2);
  std::ifstream file("/proc/stat");
  std::string line;
  unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

  if (std::getline(file, line) && line.rfind("cpu ", 0) == 0) {
    std::istringstream iss(line);
    std::string label;
    iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
  }

  unsigned long long prev_idle = idle + iowait;
  unsigned long long prev_total = user + nice + system + idle + iowait + irq + softirq + steal;

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  file.clear();
  file.seekg(0, std::ios::beg);
  std::getline(file, line);
  std::istringstream iss(line);
  std::string label;
  iss >> label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

  unsigned long long curr_idle = idle + iowait;
  unsigned long long curr_total = user + nice + system + idle + iowait + irq + softirq + steal;

  double total_diff = curr_total - prev_total;
  double idle_diff = curr_idle - prev_idle;

  if (total_diff == 0)
    return 0.0;
  return 100.0 * idle_diff / total_diff;
}

/**
 * @brief Reads the number of context switches per second.
 * @return amount of context switches per second
 */
int GetContextSwitchesPerSec() {
  std::cout << std::fixed << std::setprecision(2);
  auto read_ctx_switches = []() -> unsigned long long {
    std::ifstream file("/proc/stat");
    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("ctxt", 0) == 0) {
        unsigned long long ctxt;
        std::istringstream iss(line.substr(5));
        iss >> ctxt;
        return ctxt;
      }
    }
    return 0;
  };

  unsigned long long prev = read_ctx_switches();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  unsigned long long curr = read_ctx_switches();

  return static_cast<int>(curr - prev);
}

/**
 * @brief Reads the number of interrupts per second.
 * @return amount of interrupts per second
 */
int GetInterruptsPerSec() {
  std::cout << std::fixed << std::setprecision(2);
  auto read_interrupts = []() -> unsigned long long {
    std::ifstream file("/proc/stat");
    std::string line;
    while (std::getline(file, line)) {
      if (line.rfind("intr", 0) == 0) {
        unsigned long long intr;
        std::istringstream iss(line.substr(5));
        iss >> intr;
        return intr;
      }
    }
    return 0;
  };

  unsigned long long prev = read_interrupts();
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  unsigned long long curr = read_interrupts();

  return static_cast<int>(curr - prev);
}

}  // namespace observer::cpu
