#include "utils/monitoring.hpp"

#include <mutex>

using namespace ftxui;

namespace observer::monitoring {

/**
 * @brief Handles common events for navigation and exiting the UI
 */
enum class NavigationAction { NONE, EXIT, MENU, ARROW_UP, ARROW_DOWN };

NavigationAction HandleCommonEvents(Event event, std::atomic<bool>& running,
                                    ScreenInteractive& screen) {
  if (event == Event::Character('q') || event == Event::Character('Q')) {
    running = false;
    screen.ExitLoopClosure()();
    return NavigationAction::EXIT;
  }
  if (event == Event::Character('r') || event == Event::Character('R')) {
    running = false;
    screen.ExitLoopClosure()();
    return NavigationAction::MENU;
  }
  if (event == Event::ArrowUp) {
    running = false;
    screen.ExitLoopClosure()();
    return NavigationAction::ARROW_UP;
  }
  if (event == Event::ArrowDown) {
    running = false;
    screen.ExitLoopClosure()();
    return NavigationAction::ARROW_DOWN;
  }
  return NavigationAction::NONE;
}

/**
 * @brief Display of average CPU-temperature as a concatenated loop via CLI in ASCII-format
 */
void ShowRealTimeMinimumAsciiUIForCPU() {
  observer::cpu::SaveConfidentialCPUInfo();
  observer::utilities::GetLogger().Log("Observation: Starting minimum CPU observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  observer::graph::LiveCPUGraph cpu_graph(300);
  cpu_graph.set_max_value(100);

  std::mutex data_mutex;
  double current_temp = 0.0;
  double scale_min = 30.0;
  double scale_max = 80.0;

  // Sampler thread
  std::thread sampler([&] {
    while (running) {
      double t = observer::cpu::GetAverageCPUTemperature();

      {
        std::lock_guard<std::mutex> lock(data_mutex);
        current_temp = t;

        if (t < scale_min + 5)
          scale_min = std::max(0.0, t - 10);
        if (t > scale_max - 5)
          scale_max = std::min(120.0, t + 10);

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

    return vbox({text("=== OBSERVER: BRIEF CPU-OBSERVATION ===") | bold | center | italic |
                     color(Color::LightSkyBlue3Bis),
                 separator(),
                 text("CPU Model: " + observer::cpu::GetCPUModel()) | bold |
                     color(Color::LightSkyBlue3Bis),
                 separator(),
                 text("Temperature: " + std::to_string((int)current_temp) + " °C") | bold,
                 separator(),
                 hbox({vbox({temp_label(1.0), filler(), temp_label(0.75), filler(), temp_label(0.5),
                             filler(), temp_label(0.25), filler(), temp_label(0.0)}) |
                           size(WIDTH, EQUAL, 6),
                       separator(),
                       ftxui::graph(std::ref(cpu_graph)) | color(Color::LightSkyBlue3Bis) | flex}),
                 separator(), text("DETAILED INFO: ↑ | MENU: [R] | EXIT: [Q]") | bold | center}) |
           border;
  });

  NavigationAction next_action = NavigationAction::NONE;

  component = CatchEvent(component, [&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(component);

  observer::utilities::GetLogger().Log("Observation: Terminating minimum CPU observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  sampler.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_UP) {
    ShowRealTimeMaximumAsciiUIForCPU();
  }
}

/**
 * @brief Displays a real-time ASCII UI for CPU Observation using FTXUI.
 */
void ShowRealTimeMaximumAsciiUIForCPU() {
  observer::cpu::SaveConfidentialCPUInfo();
  observer::utilities::GetLogger().Log("Observation: Starting maximum CPU observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  std::string cpu_name = observer::cpu::GetCPUModel();
  std::mutex data_mutex;

  double avg_temp = 0.0;
  double avg_freq = 0.0;
  double idle_percent = 0.0;
  int ctx_switches = 0;
  int interrupts = 0;

  std::vector<double> temps = observer::cpu::GetAllCPUTemperatures();
  std::vector<double> freqs = observer::cpu::GetAllCPUFrequencies();
  std::vector<double> loads = observer::cpu::GetCPULoadPerCore();

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<Element> core_rows;
    core_rows.reserve(temps.size());

    for (size_t i = 0; i < temps.size(); ++i) {
      core_rows.push_back(hbox({
          text("Core " + std::to_string(i)) | size(WIDTH, EQUAL, 8),
          separator(),
          text(observer::utilities::FormatDouble(temps[i], 1) + " °C") | size(WIDTH, EQUAL, 14),
          separator(),
          text(observer::utilities::FormatAsInt(freqs[i]) + " MHz") | size(WIDTH, EQUAL, 16),
          separator(),
          text(observer::utilities::FormatDouble(loads[i], 1) + " %") | size(WIDTH, EQUAL, 10),
      }));
    }

    return vbox({text("=== OBSERVER: DETAILED CPU-OBSERVATION ===") | bold | center | italic |
                     color(Color::LightSkyBlue3Bis),
                 separator(), text("CPU: " + cpu_name) | bold | color(Color::LightSkyBlue3Bis),
                 separator(),
                 hbox({
                     text("Core") | bold | size(WIDTH, EQUAL, 8) | color(Color::LightSkyBlue3Bis),
                     text("| Temp (°C)") | bold | size(WIDTH, EQUAL, 15) |
                         color(Color::LightSkyBlue3Bis),
                     text("| Freq (MHz)") | bold | size(WIDTH, EQUAL, 17) |
                         color(Color::LightSkyBlue3Bis),
                     text("| Load (%)") | bold | size(WIDTH, EQUAL, 10) |
                         color(Color::LightSkyBlue3Bis),
                 }),
                 separator(), vbox(core_rows), separator(),
                 text("Avg Temp: " + observer::utilities::FormatDouble(avg_temp, 1) + " °C") | bold,
                 separator(),
                 text("Avg Freq: " + observer::utilities::FormatAsInt(avg_freq) + " MHz") | bold,
                 separator(),
                 text("Idle: " + observer::utilities::FormatDouble(idle_percent, 1) + "%") | bold,
                 separator(), text("Context Switches/s: " + std::to_string(ctx_switches)) | bold,
                 separator(), text("Interrupts/s: " + std::to_string(interrupts)) | bold,
                 separator(), text("BRIEF INFO: ↓ | MENU: [R] | EXIT: [Q]") | bold | center}) |
           border;
  });

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

  NavigationAction next_action = NavigationAction::NONE;

  renderer |= CatchEvent([&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating maximum CPU observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_DOWN) {
    ShowRealTimeMinimumAsciiUIForCPU();
  }
}

/**
 * @brief Displays a minimum real-time ASCII UI for RAM bservation using FTXUI.
 */
void ShowRealTimeMinimumAsciiUIForRAM() {
  observer::ram::SaveConfidentialRAMInfo();
  observer::utilities::GetLogger().Log("Observation: Starting minimum RAM observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  std::mutex data_mutex;
  double total_ram = 0.0;
  double available_ram = 0.0;
  double used_ram = 0.0;
  float used_percent = 0.0f;

  auto slider = Slider("", &used_percent, 0.0f, 100.0f, 0.1f);

  auto renderer = Renderer(slider, [&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    Color usage_color = used_percent > 85   ? Color::Red
                        : used_percent > 70 ? Color::Yellow
                                            : Color::Green;

    return vbox({text("=== OBSERVER: BRIEF RAM OBSERVATION ===") | bold | center | italic |
                     color(Color::LightSkyBlue3Bis),
                 separator(), text("Total: " + std::to_string((int)total_ram) + " MB") | bold,
                 text("Used: " + std::to_string((int)used_ram) + " MB") | bold,
                 text("Available: " + std::to_string((int)available_ram) + " MB") | bold,
                 separator(),
                 hbox({text("Usage: "), slider->Render() | flex,
                       text(" " + std::to_string((int)used_percent) + "%")}) |
                     color(usage_color) | bold,
                 separator(),
                 text("DETAILED INFO: ↑ | MENU: [R] | EXIT: [Q]") | bold | center |
                     color(Color::LightSkyBlue3Bis)}) |
           border;
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

  NavigationAction next_action = NavigationAction::NONE;

  renderer |= CatchEvent([&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating minimum RAM observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_UP) {
    ShowRealtTimeMaximumAsciiUIForRAM();
  }
}

/**
 * @brief Displays a real-time ASCII UI for RAM observation using FTXUI.
 */
void ShowRealtTimeMaximumAsciiUIForRAM() {
  observer::ram::SaveConfidentialRAMInfo();
  observer::utilities::GetLogger().Log("Observation: Starting maximum RAM observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  std::mutex data_mutex;
  double total_ram = 0.0;
  double available_ram = 0.0;
  double used_ram = 0.0;
  double used_percent = 0.0;

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    return vbox({text("=== OBSERVER: DETAILED RAM OBSERVATION ===") | bold | center | italic |
                     color(Color::LightSkyBlue3Bis),
                 separator(),
                 text("Total memory: " + std::to_string((int)total_ram) + " MB") | bold,
                 separator(),
                 text("Available memory: " + std::to_string((int)available_ram) + " MB") | bold,
                 separator(), text("Used memory: " + std::to_string((int)used_ram) + " MB") | bold,
                 separator(), text("Load: " + std::to_string((int)used_percent) + " %") | bold,
                 separator(),
                 text("BRIEF INFO: ↓ | MENU: [R] | EXIT: [Q]") | bold | center |
                     color(Color::LightSkyBlue3Bis)}) |
           border;
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

  NavigationAction next_action = NavigationAction::NONE;

  renderer |= CatchEvent([&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating maximum RAM observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_DOWN) {
    ShowRealTimeMinimumAsciiUIForRAM();
  }
}

/**
 * @brief Displays a minimum real-time ASCII UI for Storage observation using FTXUI.
 */
void ShowRealTimeMinimumAsciiUIForStorage() {
  observer::storage::SaveConfidentialStorageInfo();
  observer::utilities::GetLogger().Log("Observation: Starting minimum Storage observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  std::mutex data_mutex;
  std::vector<observer::storage::StorageInfo> storages = observer::storage::ReadAllStorageDevices();

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<Element> device_rows;
    device_rows.reserve(storages.size());

    for (size_t i = 0; i < storages.size(); i++) {
      float usage = storages[i].used_percent;
      Color bar_color = usage > 85 ? Color::Red : usage > 70 ? Color::Yellow : Color::Green;

      device_rows.push_back(
          vbox({text(storages[i].device_name + " (" + storages[i].type + ")") | bold,
                hbox({text("["), gauge(usage / 100.0f) | flex | color(bar_color), text("]"),
                      text(" " + observer::utilities::FormatDouble(usage, 1) + "%")}),
                text(observer::utilities::FormatDouble(storages[i].used_GB, 1) + " / " +
                     observer::utilities::FormatDouble(storages[i].total_GB, 1) + " GB") |
                    dim,
                separator()}));
    }

    return vbox({text("=== OBSERVER: BRIEF STORAGE OBSERVATION ===") | bold | center | italic |
                     color(Color::LightSkyBlue3Bis),
                 separator(), vbox(device_rows),
                 text("DETAILED VIEW: ↑ | MENU: [R] | EXIT: [Q]") | bold | center |
                     color(Color::LightSkyBlue3Bis)}) |
           border;
  });

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

  NavigationAction next_action = NavigationAction::NONE;

  renderer |= CatchEvent([&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating minimum Storage observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_UP) {
    ShowRealTimeMaximumAsciiUIForStorage();
  }
}

/**
 * @brief Displays a detailed real-time ASCII UI for Storage observation using FTXUI.
 */
void ShowRealTimeMaximumAsciiUIForStorage() {
  observer::storage::SaveConfidentialStorageInfo();
  observer::utilities::GetLogger().Log("Observation: Starting maximum Storage observation UI",
                                       observer::logging::LogLevel::INFO);
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running{true};

  std::mutex data_mutex;
  std::vector<observer::storage::StorageInfo> storages = observer::storage::ReadAllStorageDevices();

  Component renderer = Renderer([&] {
    std::lock_guard<std::mutex> lock(data_mutex);

    std::vector<Element> device_rows;
    device_rows.reserve(storages.size());

    for (size_t i = 0; i < storages.size(); i++) {
      device_rows.push_back(hbox({
          text(std::to_string(i + 1)) | size(WIDTH, EQUAL, 8),
          separator(),
          text(" " + storages[i].device_name) | size(WIDTH, EQUAL, 14),
          separator(),
          text(" " + storages[i].model) | size(WIDTH, EQUAL, 26),
          separator(),
          text(" " + storages[i].type) | size(WIDTH, EQUAL, 12),
          separator(),
          text(" " + observer::utilities::FormatDouble(storages[i].total_GB, 1)) |
              size(WIDTH, EQUAL, 16),
          separator(),
          text(" " + observer::utilities::FormatDouble(storages[i].used_GB, 1)) |
              size(WIDTH, EQUAL, 18),
          separator(),
          text(" " + observer::utilities::FormatDouble(storages[i].free_GB, 1)) |
              size(WIDTH, EQUAL, 18),
          separator(),
          text(" " + observer::utilities::FormatDouble(storages[i].used_percent, 1)) |
              size(WIDTH, EQUAL, 18),
          separator(),
      }));
    }

    return vbox(
               {text("=== OBSERVER: DETAILED STORAGE OBSERVATION ===") | bold | center | italic |
                    color(Color::LightSkyBlue3Bis),
                separator(),
                hbox({
                    text("Device") | bold | size(WIDTH, EQUAL, 8) | color(Color::LightSkyBlue3Bis),
                    text("| Name") | bold | size(WIDTH, EQUAL, 15) | color(Color::LightSkyBlue3Bis),
                    text("| Model") | bold | size(WIDTH, EQUAL, 27) |
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
                text("BRIEF VIEW: ↓ | MENU: [R] | EXIT: [Q]") | bold | center |
                    color(Color::LightSkyBlue3Bis)}) |
           border;
  });

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

  NavigationAction next_action = NavigationAction::NONE;

  renderer |= CatchEvent([&](Event event) {
    next_action = HandleCommonEvents(event, running, screen);
    return next_action != NavigationAction::NONE;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating maximum Storage observation UI",
                                       observer::logging::LogLevel::WARNING);

  running = false;
  updater.join();

  if (next_action == NavigationAction::MENU) {
    OptionDetectionForStartingMenu();
  } else if (next_action == NavigationAction::ARROW_DOWN) {
    ShowRealTimeMinimumAsciiUIForStorage();
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
                   color(Color::LightSkyBlue3Bis),
               separator(),
               text("=== INITIAL STARTING PAGE ===") | bold | center | italic |
                   color(Color::LightSkyBlue3Bis),
               separator(),
               text("DATE:  " + date) | bold | color(Color::White),
               separator(),
               text("CURRENT USER: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
               separator(),
               text("OPTIONS: [1] CPU | [2] MEMORY | [3] STORAGE") | bold | color(Color::White),
               separator(),
               text("=== EXIT: PRESS [Q] ===") | bold | center | color(Color::LightSkyBlue3Bis),
           }) |
           border;
  });

  int selected_option = 0;

  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('1')) {
      selected_option = 1;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('2')) {
      selected_option = 2;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('3')) {
      selected_option = 3;
      screen.ExitLoopClosure()();
      return true;
    }
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      selected_option = 0;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);

  observer::utilities::GetLogger().Log("Observation: Terminating observation UI",
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