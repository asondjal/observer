#include "utils/monitoring.hpp"

using namespace ftxui;

namespace observer::monitoring {

void ShowRealTimeAsciiUIForCPU() {
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;

  std::string cpu_name = observer::cpu::GetCPUModel();
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  double avg_temp = 0.0;
  double avg_freq = 0.0000;
  double idle_percent = 0.0;
  int ctx_switches = 0;
  int interrupts = 0;

  std::vector<double> temps = observer::cpu::GetAllCPUTemperatures();
  std::vector<double> freqs = observer::cpu::GetAllCPUFrequencies();
  std::vector<double> loads = observer::cpu::GetCPULoadPerCore();

  Component renderer = Renderer([&] {
    std::vector<Element> core_rows;
    for (size_t i = 0; i < temps.size(); ++i) {
      core_rows.push_back(hbox({
          text("Core " + std::to_string(i)) | size(WIDTH, EQUAL, 8) | color(Color::White),
          separator(),
          text(std::to_string(temps[i]) + " °C") | size(WIDTH, EQUAL, 14) | color(Color::White),
          separator(),
          text(std::to_string(freqs[i]) + " MHz") | size(WIDTH, EQUAL, 16) | color(Color::White),
          separator(),
          text(std::to_string(loads[i]) + " %") | size(WIDTH, EQUAL, 10) | color(Color::White),
      }));
    }

    return vbox(
               {text("=== OBSERVER: CPU MONITORING ===") | bold | center | color(Color::DarkOrange),
                separator(),
                text("CPU: " + cpu_name) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                hbox({
                    text("Core") | bold | size(WIDTH, EQUAL, 8) | color(Color::White),
                    text("| Temp (°C)") | bold | size(WIDTH, EQUAL, 14) | color(Color::White),
                    text("| Freq (MHz)") | bold | size(WIDTH, EQUAL, 16) | color(Color::White),
                    text("| Load (%)") | bold | size(WIDTH, EQUAL, 10) | color(Color::White),
                }),
                separator(),
                vbox(core_rows),
                separator(),
                text("Average Temperature: " + std::to_string(avg_temp) + " °C") | bold | color(Color::White),
                separator(),
                text("Average Frequency: " + std::to_string(avg_freq) + " MHz") | bold | color(Color::White),
                separator(),
                text("Idle: " + std::to_string(idle_percent) + "%") | bold | color(Color::White),
                separator(),
                text("Context Switches/s: " + std::to_string(ctx_switches)) | bold | color(Color::White),
                separator(),
                text("Interrupts/s: " + std::to_string(interrupts)) | bold | color(Color::White),
                separator(),
                text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("EXIT: Press [Q] or [q]") | bold | center | color(Color::DarkOrange)}) |
           borderLight;
  });

  // Background-thread for live-updates
  std::thread updater([&] {
    while (running) {
      temps = observer::cpu::GetAllCPUTemperatures();
      freqs = observer::cpu::GetAllCPUFrequencies();
      loads = observer::cpu::GetCPULoadPerCore();
      avg_temp = observer::cpu::GetAverageCPUTemperature();
      avg_freq = observer::cpu::GetAverageCPUFrequency();
      idle_percent = observer::cpu::GetIdlePercentage();
      ctx_switches = observer::cpu::GetContextSwitchesPerSec();
      interrupts = observer::cpu::GetInterruptsPerSec();
      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  // Terminate the thread
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
  updater.join();
}

void ShowRealtTimeAsciiUIForRAM() {
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  double total_ram = 0.0000;
  double available_ram = 0.0000;
  double used_ram = 0.0000;

    Component renderer = Renderer([&] {

    return vbox(
               {text("=== OBSERVER: RAM-MONITORING ===") | bold | center | color(Color::DarkOrange),
                separator(),
                text("Total memory: " + std::to_string(total_ram) + " MB") | bold | color(Color::White),
                separator(),
                text("Available memory: " + std::to_string(available_ram) + " MB") | bold | color(Color::White),
                separator(),
                text("Used memory: " + std::to_string(used_ram) + " %") | bold | color(Color::White),
                separator(),
                text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("EXIT: Press [Q] or [q]") | bold | center | color(Color::DarkOrange)}) |
           borderLight;
  });

  // Background-thread for live-updates
  std::thread updater([&] {
    while (running) {
      observer::ram::RAMInfo current_state = observer::ram::ReadRAMInfo();
      total_ram = current_state.total_MB;
      available_ram = current_state.available_MB;
      used_ram = current_state.used_percent;
      screen.PostEvent(Event::Custom);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  });

  // Terminate the thread
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
  updater.join();

}

void ShowRealTimeAsciiUIForStorage() {
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string current_user = observer::utilities::GetCurrentUser();

  std::string device_name;
  std::string model;
  std::string type;
  double total_GB = 0.0;
  double used_GB = 0.0;
  double free_GB = 0.0;
  double used_percent = 0.0;

  std::vector<observer::storage::StorageInfo> storages = observer::storage::ReadAllStorageDevices();
  Component renderer = Renderer([&] {
    //   for (size_t i = 0; i < storages.size(); ++i) {
    //   storages.push_back(hbox({
    //       text("Core " + std::to_string(i)) | size(WIDTH, EQUAL, 8) | color(Color::White),
    //       separator(),
    //       text(std::to_string(temps[i]) + " °C") | size(WIDTH, EQUAL, 14) | color(Color::White),
    //       separator(),
    //       text(std::to_string(freqs[i]) + " MHz") | size(WIDTH, EQUAL, 16) | color(Color::White),
    //       separator(),
    //       text(std::to_string(loads[i]) + " %") | size(WIDTH, EQUAL, 10) | color(Color::White),
    //   }));
    // }

    return vbox(
               {text("=== OBSERVER: STORAGE-MONITORING ===") | bold | center | color(Color::DarkOrange),
                separator(),
                text("Observation started: " + date) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("Current user: " + current_user) | bold | color(Color::LightSkyBlue3Bis),
                separator(),
                text("EXIT: Press [Q] or [q]") | bold | center | color(Color::DarkOrange)}) |
           borderLight;
  });
    // Terminate the process
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();  // Close the screen
      return true;
    }
    return false;
  });

  screen.Loop(renderer);

}

void GetUserChoiceFromInitialDataUI(int user_choice) {
  switch(user_choice) {
    case 1:
    observer::monitoring::ShowRealTimeAsciiUIForCPU();
    break;
    case 2:
    observer::monitoring::ShowRealtTimeAsciiUIForRAM();
    break;
    case 3:
    observer::monitoring::ShowRealTimeAsciiUIForStorage();
    break;
    default:
    std::cout << "User didn't insert an option!" << std::endl;
  }
}

void ShowInitialData() {
  auto screen = ScreenInteractive::Fullscreen();
  bool running = true;

  // Preprocessing of the data
  std::string current_user = observer::utilities::GetCurrentUser();
  std::string date = observer::utilities::GetSystemTimestamp();

  Component renderer = Renderer([&] {
    return vbox({
               text("OBSERVER SYSTEM MONITOR") | bold | center | color(Color::DarkOrange),
               separator(),
               text("INITIAL STARTING PAGE") | bold | center | color(Color::DarkOrange),
               separator(),
               text("DATE:  " + date) | bold | color(Color::White),
               separator(),
               text("CURRENT USER: " + current_user) | bold | color(Color::DarkOrange),
               separator(),
               text("OPTIONS: [1] CPU-Monitoring, [2] Storage-Monitoring, [3] Memory-Monitoring" ) | bold | color(Color::White),
               separator(),
               text("SELECT ONE OPTION: [1], [2] or [3]") | bold | color(Color::IndianRed1),
               separator(),
               text("PRESS [Q] OR [q]") | bold | center | color(Color::DarkOrange),
               separator(),
           }) |
           border;
  });

  // Terminate the process
  renderer |= CatchEvent([&](Event event) {
    if (event == Event::Character('q') || event == Event::Character('Q')) {
      running = false;
      screen.ExitLoopClosure()();  // Close the screen
      return true;
    }
    return false;
  });

  screen.Loop(renderer);
}

}  // namespace observer::monitoring