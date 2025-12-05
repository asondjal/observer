// This code is based on the example provided in the FTXUI library:

#ifndef INC_UTILS_GRAPH_HPP_
#define INC_UTILS_GRAPH_HPP_

#include <atomic>                  // for atomic
#include <chrono>                  // for operator""s, chrono_literals
#include <cmath>                   // for sin
#include <ftxui/dom/elements.hpp>  // for graph, operator|, separator, color, Element, vbox, flex, inverted, operator|=, Fit, hbox, size, border, GREATER_THAN, HEIGHT
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <functional>               // for ref, reference_wrapper
#include <iostream>                 // for cout, ostream
#include <memory>                   // for shared_ptr
#include <string>                   // for operator<<, string
#include <thread>                   // for sleep_for
#include <vector>                   // for vector

using namespace ftxui;
using namespace std::chrono_literals;
 
#include "ftxui/dom/node.hpp"  // for Render
#include "ftxui/screen/color.hpp"  // for Color, Color::BlueLight, Color::RedLight, Color::YellowLight, ftxui

#include "utils/cpu.hpp"  // for GetCPULoadPerCore
#include "utils/ram.hpp"
#include "utils/storage.hpp"
#include "utils/utilities.hpp"  // for GetSystemTimestamp, GetCurrentUser

namespace observer::graph {
    class LiveCPUGraph {
public:
    LiveCPUGraph(size_t buffer_size = 500);

    void push(int value);

    std::vector<int> operator()(int width, int height) const ;

    void set_max_value(int m);

private:
    std::vector<int> buffer;
    std::atomic<size_t> head;
    int max_value = 100;  // typical max CPU temperature
};

double simulate_cpu_temp();

}

#endif /* INC_UTILS_GRAPH_HPP_ */
