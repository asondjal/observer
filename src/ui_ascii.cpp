#include "utils/ui_ascii.hpp"

using namespace ftxui;

namespace observer {

void ShowAsciiUI(bool realtime) {
  if (realtime) {
    std::cout << "REAL-TIME mode not yet implemented.\n";
    return;
  }

  // ASCII output
  std::string cpu_name = observer::cpu::GetCPUInfo();
  std::vector<double> temps = observer::cpu::GetAllCPUTemperatures();
  double avg_temp = observer::cpu::GetAverageCPUTemperature();

  std::vector<Element> temp_bars;
  for (size_t i = 0; i < temps.size(); ++i) {
    temp_bars.push_back(hbox({
        text("[" + std::to_string(i) + "] "),
        gauge(temps[i] / 100.0),
        text(" " + std::to_string(temps[i]) + "°C"),
    }));
  }

  auto layout = vbox({
                    text("OBSERVER SYSTEM MONITOR") | bold | center,
                    separator(),
                    text("Current date: " + observer::utilities::GetSystemTimestamp()),
                    separator(),
                    text("CPU: " + cpu_name),
                    separator(),
                    text("Temperatures:"),
                    vbox(temp_bars),
                    separator(),
                    text("Average Temperature: " + std::to_string(avg_temp) + " °C"),
                    vbox({
                        separator(),
                        text(" "),
                        separator(),
                    }),
                }) |
                border;

  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(layout));
  Render(screen, layout);
  std::cout << screen.ToString() << std::endl;

  std::cout << "\nPress Enter to exit..." << std::endl;
  std::cin.ignore();
  std::cin.get();
}
}  // namespace observer