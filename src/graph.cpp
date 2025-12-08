#include "utils/graph.hpp"

namespace observer::graph {

LiveCPUGraph::LiveCPUGraph(size_t buffer_size) : buffer(buffer_size, 0), head(0) {}

void LiveCPUGraph::push(int value) {
  buffer[head % buffer.size()] = value;
  head++;
}

/**
 * @brief Generate a graph representation of the CPU temperature data.
 * @param width The width of the graph.
 * @param height The height of the graph.
 * @return A vector of integers representing the scaled CPU temperature values.
 */
std::vector<int> LiveCPUGraph::operator()(int width, int height) const {
  std::vector<int> output(width);

  // latest "width" values
  size_t total = head;
  for (int i = 0; i < width; i++) {
    size_t idx = (total + i - width) % buffer.size();
    int v = buffer[idx];

    // scale to height
    int y = (v * height) / max_value;
    if (y < 0)
      y = 0;
    if (y >= height)
      y = height - 1;

    output[i] = y;
  }

  return output;
}

void LiveCPUGraph::set_max_value(int m) { max_value = m; }

/**
 * @brief Simulate the CPU temperature for testing purposes.
 * @return A simulated CPU temperature value.
 */
double simulate_cpu_temp() {
  using namespace std::chrono;
  auto t = observer::cpu::GetAverageCPUTemperature();
  return 50 + 10 * std::sin(t);
}
}  // namespace observer::graph