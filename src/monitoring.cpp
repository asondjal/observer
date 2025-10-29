#include "utils/monitoring.hpp"

using namespace ftxui;

namespace observer::monitoring {

void ShowInitialData() {
  auto screen = ScreenInteractive::Fullscreen();
  bool running = true;

  // Preprocessing of the data
  std::string date = observer::utilities::GetSystemTimestamp();
  std::string cpu_name = observer::cpu::GetCPUInfo();
  auto temps = observer::cpu::GetAllCPUTemperatures();
  double avg_temp = observer::cpu::GetAverageCPUTemperature();

  // Setup for display of the data
  std::vector<Element> temp_bars;
  for (size_t i = 0; i < temps.size(); ++i) {
    temp_bars.push_back(hbox({text("[" + std::to_string(i) + "] "), gauge(temps[i] / 100.0),
                              text(" " + std::to_string(temps[i]) + "°C")}));
  }

  Component renderer = Renderer([&] {
    return vbox({
               text("OBSERVER SYSTEM MONITOR") | bold | center | color(Color::DarkOrange),
               separator(),
               text("Current time: " + date),
               separator(),
               text("CPU: " + cpu_name),
               separator(),
               text("Temperatures:"),
               vbox(temp_bars),
               separator(),
               text("Average Temperature: " + std::to_string(avg_temp) + " °C"),
               separator(),
               text("Press [Q] or [q]") | bold | center | color(Color::DarkOrange),
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

void ShowRealTimeAsciiUIForCPU() {
  auto screen = ScreenInteractive::Fullscreen();
  std::atomic<bool> running = true;

  std::string date = observer::utilities::GetSystemTimestamp();
  double avg_temp = 0.0;
  std::vector<double> temps;
  std::string cpu_name = observer::cpu::GetCPUInfo();

  Component renderer = Renderer([&] {
    std::vector<Element> temp_bars;
    for (size_t i = 0; i < temps.size(); ++i) {
      temp_bars.push_back(hbox({
          text("Core " + std::to_string(i) + ": "),
          gauge(temps[i] / 100.0),
          text(" " + std::to_string(temps[i]) + "°C"),
      }));
    }

    return vbox({
               text("OBSERVER REAL-TIME MONITOR") | bold | center | color(Color::DarkOrange),
               separator(),
               text("Current time: " + date),
               separator(),
               text("CPU: " + cpu_name),
               separator(),
               vbox(temp_bars),
               separator(),
               text("Average Temperature: " + std::to_string(avg_temp) + " °C"),
               separator(),
               text("Press [Q] or [q]") | bold | center | color(Color::DarkOrange),
               separator(),
           }) |
           border;
  });

  // Background-Thread for live-updates
  std::thread updater([&] {
    while (running) {
      temps = observer::cpu::GetAllCPUTemperatures();
      avg_temp = observer::cpu::GetAverageCPUTemperature();
      screen.PostEvent(Event::Custom);  // Refresh
      std::this_thread::sleep_for(
          std::chrono::milliseconds(1000));  // Sampling rate: Listen once every 1s
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
}  // namespace observer::monitoring