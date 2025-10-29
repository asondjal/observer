#include "utils/cpu.hpp"

using namespace ftxui;

namespace observer::cpu {

const std::string cpu_path = "/proc/cpuinfo";

/**
 * @brief Verbose information about the CPU
 * @return Verbose information
 */
const std::string GetVerboseCPUInfo() { return observer::utilities::GetVerboseInfo(cpu_path); }

/**
 * @brief Basic information from the CPU
 * @return Model Name
 */
const std::string GetCPUInfo() {
  std::ifstream cpuinfo(cpu_path);
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
            std::cout << std::fixed << std::setprecision(4);
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
 * @return Average temperature for the CPU with an accuracy of 4 decimals
 */
double GetAverageCPUTemperature() {
  std::cout << std::fixed << std::setprecision(4);
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
  std::cout << std::fixed << std::setprecision(4);
  for (std::size_t i = 0; i < freqs.size(); i++) {
    std::cout << "Core " << i << ": " << freqs[i] << " MHz" << std::endl;
  }
}

/**
 * @brief Calculation of the average frequency altogether
 * @return Average frequency with an accuracy of 4 decimals
 */
double GetAverageCPUFrequency() {
  std::cout << std::fixed << std::setprecision(4);
  std::vector<double> freqs = GetAllCPUFrequencies();
  if (freqs.empty())
    return 0.0;
  double sum = 0.0;
  for (double f : freqs) {
    sum += f;
  }
  return sum / freqs.size();
}

}  // namespace observer::cpu
