#ifndef INC_TESTS_TEST_CPU_HPP_
#define INC_TESTS_TEST_CPU_HPP_

#include <cassert>
#include <chrono>
#include <cmath>
#include <thread>

#include "utils/cpu.hpp"
#include "utils/read_data.hpp"

void TestCPUInfo();
void TestCPUTemperatures();
void TestCPUFrequencies();
void TestCPULoad();
void TestCPUIdle();

#endif /* INC_TESTS_TEST_CPU_HPP_ */
