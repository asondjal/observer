#include "utils/monitoring.hpp"

#include <mutex>

using namespace ftxui;

namespace observer::monitoring {

/**
 * @brief Display of average CPU-temperature as a concatenated loop via CLI in ASCII-format
 */
void ShowRealTimeMinimumAsciiUIForCPU() {
  observer::utilities::GetLogger().Log("Monitoring: Starting minimum CPU monitoring UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;

  observer::graph::LiveCPUGraph cpu_graph(300);
  cpu_graph.set_max_value(100);
  
  std::mutex data_mutex;
  double current_temp = 0.0;
  double scale_min = 30.0;
  double scale_max = 80.0;

  std::thread sampler([&] {
    while (running) {
      double t = observer::cpu::GetAverageCPUTemperature();
      
      {
        std::lock_guard<std::mutex> lock(data_mutex);
        current_temp = t;
        
        // Adaptive scaling
        if (t < scale_min + 5) scale_min = std::max(0.0, t - 10);
        if (t > scale_max - 5) scale_max = std::min(120.0, t + 10);
        
        // Normalize to 0-100
        double normalized = ((t - scale_min) / (scale_max - scale_min)) * 100.0;
        cpu_graph.push((int)std::clamp(normalized, 0.0, 100.0));
      }

      screen.Post(Event::Custom);
      std::this_thread::sleep_for(100ms);
    }
  });

  Component component = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);
    
    auto temp_label = [&](double pct) {
      int temp = (int)(scale_min + (scale_max - scale_min) * pct);
      return text(std::to_string(temp) + "°C") | dim;
    };

    return vbox({
      text("=== OBSERVER: MINIMUM CPU-MONITORING ===") | bold | center | italic |
        color(Color::LightSkyBlue3Bis),
      separator(),
      text("CPU Model: " + observer::cpu::GetCPUModel()) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      text("Temperature: " + std::to_string((int)current_temp) + " °C") | bold,
      separator(),
      hbox({
        vbox({
          temp_label(1.0), filler(), temp_label(0.75), filler(),
          temp_label(0.5), filler(), temp_label(0.25), filler(), temp_label(0.0)
        }) | size(WIDTH, EQUAL, 6),
        separator(),
        ftxui::graph(std::ref(cpu_graph)) | color(Color::LightSkyBlue3Bis) | flex
      }),
      separator(),
      text("MAXIMUM INFO: ↑ | MENU: [R] | EXIT: [Q]") | bold | center
    }) | border;
  });

  bool should_return_to_menu = false;
  bool should_show_maximum = false;

  component = CatchEvent(component, [&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('r') || event == Event::Character('R')) {
      running = false;
      should_return_to_menu = true;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::ArrowUp) {
      running = false;
      should_show_maximum = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(component);

  observer::utilities::GetLogger().Log("Monitoring: Terminating minimum CPU monitoring UI",
                                       observer::logging::LogLevel::WARNING);
  running = false;
  sampler.join();

  if (should_return_to_menu) {
    OptionDetectionForStartingMenu();
  } else if (should_show_maximum) {
    ShowRealTimeMaximumAsciiUIForCPU();
  }
}


/**
 * @brief Displays a real-time ASCII UI for CPU monitoring using FTXUI.
 */
void ShowRealTimeMaximumAsciiUIForCPU() {
  observer::cpu::SaveConfidentialCPUInfo();
  observer::utilities::GetLogger().Log("Monitoring: Starting maximum CPU monitoring UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;

  std::string cpu_name = observer::cpu::GetCPUModel();
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  std::mutex data_mutex;

  double avg_temp = 0.0;
  double avg_freq = 0.0;
  double idle_percent = 0.0;
  int ctx_switches = 0;
  int interrupts = 0;

  std::vector<double> temps = observer::cpu::GetAllCPUTemperatures();
  std::vector<double> freqs = observer::cpu::GetAllCPUFrequencies();
  std::vector<double> loads = observer::cpu::GetCPULoadPerCore();

  auto format_double = [](double value, int precision = 2) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
  };

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<Element> core_rows;
    for (size_t i = 0; i < temps.size(); ++i) {
      core_rows.push_back(hbox({
          text("Core " + std::to_string(i)) | size(WIDTH, EQUAL, 8),
          separator(),
          text(format_double(temps[i], 1) + " °C") | size(WIDTH, EQUAL, 14),
          separator(),
          text(format_double(freqs[i], 0) + " MHz") | size(WIDTH, EQUAL, 16),
          separator(),
          text(format_double(loads[i], 1) + " %") | size(WIDTH, EQUAL, 10),
      }));
    }

    return vbox({
      text("=== OBSERVER: DETAILED CPU-MONITORING ===") | bold | center | italic | color(Color::DarkOrange),
      separator(),
      text("CPU: " + cpu_name) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      hbox({
          text("Core") | bold | size(WIDTH, EQUAL, 8) | color(Color::LightSkyBlue3Bis),
          text("| Temp (°C)") | bold | size(WIDTH, EQUAL, 15) | color(Color::LightSkyBlue3Bis),
          text("| Freq (MHz)") | bold | size(WIDTH, EQUAL, 17) | color(Color::LightSkyBlue3Bis),
          text("| Load (%)") | bold | size(WIDTH, EQUAL, 10) | color(Color::LightSkyBlue3Bis),
      }),
      separator(),
      vbox(core_rows),
      separator(),
      text("Average Temperature: " + observer::utilities::FormatDouble(avg_temp, 1) + " °C") | bold,
      separator(),
      text("Average Frequency: " + observer::utilities::FormatDouble(avg_freq, 0) + " MHz") | bold,
      separator(),
      text("Idle: " + observer::utilities::FormatDouble(idle_percent, 1) + "%") | bold,
      separator(),
      text("Context Switches/s: " + observer::utilities::FormatDouble(ctx_switches)) | bold,
      separator(),
      text("Interrupts/s: " + observer::utilities::FormatDouble(interrupts)) | bold,
      separator(),
      text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      text("MINIMUM INFO: ↓ | MENU: [R] | EXIT: [Q]") | bold | center
    }) | border;
  });

  // Background-thread for live-updates
  std::thread updater([&] {
    while (running) {
      auto new_temps = observer::cpu::GetAllCPUTemperatures();
      auto new_freqs = observer::cpu::GetAllCPUFrequencies();
      auto new_loads = observer::cpu::GetCPULoadPerCore();
      double new_avg_temp = observer::cpu::GetAverageCPUTemperature();
      double new_avg_freq = observer::cpu::GetAverageCPUFrequency();
      double new_idle_percent = observer::cpu::GetIdlePercentage();
      int new_ctx_switches = observer::cpu::GetContextSwitchesPerSec();
      int new_interrupts = observer::cpu::GetInterruptsPerSec();

      {
        std::lock_guard<std::mutex> lock(data_mutex);
        temps = std::move(new_temps);
        freqs = std::move(new_freqs);
        loads = std::move(new_loads);
        avg_temp = new_avg_temp;
        avg_freq = new_avg_freq;
        idle_percent = new_idle_percent;
        ctx_switches = new_ctx_switches;
        interrupts = new_interrupts;
      }

      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  bool should_return_to_menu = false;
  bool should_show_minimum = false;

  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('r') || event == Event::Character('R')) {
      running = false;
      should_return_to_menu = true;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::ArrowDown) {
      running = false;
      should_show_minimum = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
  observer::utilities::GetLogger().Log("Monitoring: Terminating maximum CPU monitoring UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (should_return_to_menu) {
    OptionDetectionForStartingMenu();
  } else if (should_show_minimum) {
    ShowRealTimeMinimumAsciiUIForCPU();
  }
}
/**
 * @brief Displays a minimum real-time ASCII UI for RAM monitoring using FTXUI.
 */
void ShowRealTimeMinimumAsciiUIForRAM() {
  observer::ram::SaveConfidentialRAMInfo();
  observer::utilities::GetLogger().Log("Monitoring: Starting minimum RAM monitoring UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;
  
  std::mutex data_mutex;
  double total_ram = 0.0;
  double available_ram = 0.0;
  double used_ram = 0.0;
  float used_percent = 0.0f;

  auto slider = Slider("", &used_percent, 0.0f, 100.0f, 0.1f);

  auto renderer = Renderer(slider, [&] {
    std::lock_guard<std::mutex> lock(data_mutex);
    
    Color usage_color = used_percent > 85 ? Color::Red :
                        used_percent > 70 ? Color::Yellow :
                        Color::Green;
    
    return vbox({
      text("=== OBSERVER: MINIMUM RAM MONITORING ===") | bold | center | italic | color(Color::DarkOrange),
      separator(),
      text("Total: " + std::to_string((int)total_ram) + " MB") | bold,
      text("Used: " + std::to_string((int)used_ram) + " MB") | bold,
      text("Available: " + std::to_string((int)available_ram) + " MB") | bold,
      separator(),
      hbox({
        text("Usage: "),
        slider->Render() | flex,
        text(" " + std::to_string((int)used_percent) + "%")
      }) | color(usage_color) | bold,
      separator(),
      text("MAXIMUM INFO: ↑ | MENU: [R] | EXIT: [Q]") | bold | center
    }) | border;
  });

  std::thread updater([&] {
    while (running) {
      observer::ram::RAMInfo current_state = observer::ram::ReadRAMInfo();
      {
        std::lock_guard<std::mutex> lock(data_mutex);
        total_ram = current_state.total_MB;
        available_ram = current_state.available_MB;
        used_ram = current_state.used_MB;
        used_percent = (float)current_state.used_percent;
      }
      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  bool should_return_to_menu = false;
  bool should_show_maximum = false;

  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('r') || event == Event::Character('R')) {
      running = false;
      should_return_to_menu = true;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::ArrowUp) {
      running = false;
      should_show_maximum = true;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Monitoring: Terminating minimum RAM monitoring UI",
                                       observer::logging::LogLevel::WARNING);
  running = false;
  updater.join();

  if (should_return_to_menu) {
    OptionDetectionForStartingMenu();
  } else if (should_show_maximum) {  // FIXED: check correct flag
    ShowRealtTimeMaximumAsciiUIForRAM();
  }
}

/**
 * @brief Displays a real-time ASCII UI for RAM monitoring using FTXUI.
 */
void ShowRealtTimeMaximumAsciiUIForRAM() {
  observer::ram::SaveConfidentialRAMInfo();
  observer::utilities::GetLogger().Log("Monitoring: Starting maximum RAM monitoring UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  std::mutex data_mutex;
  double total_ram = 0.0;
  double available_ram = 0.0;
  double used_ram = 0.0;
  double used_percent = 0.0;

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    return vbox({
      text("=== OBSERVER: MAXIMUM RAM MONITORING ===") | bold | center | italic | color(Color::DarkOrange),
      separator(),
      text("Total memory: " + std::to_string((int)total_ram) + " MB") | bold,
      separator(),
      text("Available memory: " + std::to_string((int)available_ram) + " MB") | bold,
      separator(),
      text("Used memory: " + std::to_string((int)used_ram) + " MB") | bold,
      separator(),
      text("Load: " + std::to_string((int)used_percent) + " %") | bold,
      separator(),
      text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
      separator(),
      text("MINIMUM INFO: ↓ | MENU: [R] | EXIT: [Q]") | bold | center
    }) | border;
  });

  std::thread updater([&] {
    while (running) {
      observer::ram::RAMInfo current_state = observer::ram::ReadRAMInfo();
      {
        std::lock_guard<std::mutex> lock(data_mutex);
        total_ram = current_state.total_MB;
        available_ram = current_state.available_MB;
        used_ram = current_state.used_MB;
        used_percent = current_state.used_percent;
      }
      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  });

  bool should_return_to_menu = false;
  bool should_show_minimum = false;

  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('r') || event == Event::Character('R')) {
      running = false;
      should_return_to_menu = true;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::ArrowDown) {
      running = false;
      should_show_minimum = true; 
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
  
  observer::utilities::GetLogger().Log("Monitoring: Terminating maximum RAM monitoring UI",
                                       observer::logging::LogLevel::WARNING);
  running = false;
  updater.join();

  if (should_return_to_menu) {
    OptionDetectionForStartingMenu();
  } else if (should_show_minimum) { 
    ShowRealTimeMinimumAsciiUIForRAM();
  }
}
/**
 * @brief Displays a real-time ASCII UI for Storage monitoring using FTXUI.
 */
void ShowRealTimeMaximumAsciiUIForStorage() {
  observer::storage::SaveConfidentialStorageInfo();
  observer::utilities::GetLogger().Log("Monitoring: Starting maximum Storage monitoring UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  std::mutex data_mutex;
  std::vector<observer::storage::StorageInfo> storages = observer::storage::ReadAllStorageDevices();

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<Element> device_rows;
    for (size_t i = 0; i < storages.size(); i++) {
      device_rows.push_back(hbox({
          text(std::to_string(i + 1)) | size(WIDTH, EQUAL, 8) | color(Color::White),
          separator(),
          text(" " + storages[i].device_name) | size(WIDTH, EQUAL, 14) | color(Color::White),
          separator(),
          text(" " + storages[i].model) | size(WIDTH, EQUAL, 26) | color(Color::White),
          separator(),
          text(" " + storages[i].type) | size(WIDTH, EQUAL, 12) | color(Color::White),
          separator(),
          text(" " + std::to_string(storages[i].total_GB)) | size(WIDTH, EQUAL, 16) |
              color(Color::White),
          separator(),
          text(" " + std::to_string(storages[i].used_GB)) | size(WIDTH, EQUAL, 18) |
              color(Color::White),
          separator(),
          text(" " + std::to_string(storages[i].free_GB)) | size(WIDTH, EQUAL, 18) |
              color(Color::White),
          separator(),
          text(" " + std::to_string(storages[i].used_percent)) | size(WIDTH, EQUAL, 18) |
              color(Color::White),
          separator(),
      }));
    }

    return vbox(
               {text("=== OBSERVER: DETAILED STORAGE-MONITORING ===") | bold | center | italic |
                    color(Color::DarkOrange),
                separator(),
                hbox({
                    text("Device") | bold | size(WIDTH, EQUAL, 8) | color(Color::LightSkyBlue3Bis),
                    text("| Name") | bold | size(WIDTH, EQUAL, 15) | color(Color::LightSkyBlue3Bis),
                    text("| Model ") | bold | size(WIDTH, EQUAL, 27) |
                        color(Color::LightSkyBlue3Bis),
                    text("| Type") | bold | size(WIDTH, EQUAL, 13) | color(Color::LightSkyBlue3Bis),
                    text("| Total (GB)") | bold | size(WIDTH, EQUAL, 17) |
                        color(Color::LightSkyBlue3Bis),
                    text("| Used (GB)") | bold | size(WIDTH, EQUAL, 19) |
                        color(Color::LightSkyBlue3Bis),
                    text("| Free (GB)") | bold | size(WIDTH, EQUAL, 19) |
                        color(Color::LightSkyBlue3Bis),
                    text("| Load (%)") | bold | size(WIDTH, EQUAL, 10) |
                        color(Color::LightSkyBlue3Bis),
                }),
                separator(), vbox(device_rows), separator(),
                text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("=== RETURN TO STARTING MENU: Press [R] or [r] ===") | bold | center | italic |
                    color(Color::DarkOrange),
                separator(),
                text("=== EXIT: Press [Q] or [q] ===") | bold | center | italic |
                    color(Color::DarkOrange)}) |
           borderLight;
  });

  // Background-thread for live-updates
  std::thread updater([&] {
    while (running) {
      auto new_storages = observer::storage::ReadAllStorageDevices();

      {
        std::lock_guard<std::mutex> lock(data_mutex);
        storages = std::move(new_storages);
      }

      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  bool should_return_to_menu = false;

  // Terminate the process
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }

    if (event == Event::Character('r') || event == Event::Character('R')) {
      running = false;
      should_return_to_menu = true;
      screen.ExitLoopClosure()();
      return true;
    }

    return false;
  });

  screen.Loop(renderer);
  observer::utilities::GetLogger().Log("Monitoring: Terminating maximum Storage monitoring UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (should_return_to_menu) {
    OptionDetectionForStartingMenu();
  }
}

/**
 * @brief Starting menu displayed at the launch of the application.
 * @return int representing the user's selected option.
 */
int OptionDetectionForStartingMenu() {
  auto screen = ScreenInteractive::Fullscreen();

  std::string current_user = observer::utilities::GetCurrentUser();
  std::string date = observer::utilities::GetSystemTimestamp();

  Component renderer = Renderer([&] {
    return vbox({
               text("=== OBSERVER SYSTEM MONITOR ===") | bold | center | italic |
                   color(Color::DarkOrange),
               separator(),
               text("=== INITIAL STARTING PAGE ===") | bold | center | italic |
                   color(Color::DarkOrange),
               separator(),
               text("DATE:  " + date) | bold | color(Color::White),
               separator(),
               text("CURRENT USER: " + current_user) | bold | color(Color::DarkOrange),
               separator(),
               text("SELECTABLE OPTIONS: [1] CPU-MONITORING, [2] MEMORY-MONITORING, [3] "
                    "STORAGE-MONITORING") |
                   bold | color(Color::White),
               separator(),
               text("=== EXIT: PRESS [Q] OR [q] ===") | bold | center | color(Color::DarkOrange),
           }) |
           border;
  });

  // Terminate the process
  int selected_option = 0;
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('1')) {
      selected_option = 1;
      screen.ExitLoopClosure()();
      return true;
    } else if (event == Event::Character('2')) {
      selected_option = 2;
      screen.ExitLoopClosure()();
      return true;
    } else if (event == Event::Character('3')) {
      selected_option = 3;
      screen.ExitLoopClosure()();
      return true;
    } else if (event == Event::Character('q') || event == Event::Character('Q')) {
      selected_option = 0;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
  observer::utilities::GetLogger().Log("Monitoring: Terminating monitoring UI",
                                       observer::logging::LogLevel::WARNING);

  if (selected_option == 1) {
    ShowRealTimeMaximumAsciiUIForCPU();
  } else if (selected_option == 2) {
    ShowRealtTimeMaximumAsciiUIForRAM();
  } else if (selected_option == 3) {
    ShowRealTimeMaximumAsciiUIForStorage();
  }

  return selected_option;
}

}  // namespace observer::monitoring