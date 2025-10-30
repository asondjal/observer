#ifndef INC_TESTS_TEST_UTILITIES_HPP_
#define INC_TESTS_TEST_UTILITIES_HPP_

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "utils/logging.hpp"
#include "utils/utilities.hpp"

void TestCurrentUser();
void TestFileOwner();
void TestSecureFileOwnership();
void TestGetVerboseInfo();
void TestSaveConfidentialData();
void TestGetTimeStamp();

#endif /*INC_TESTS_TEST_UTILITIES_HPP_ */
