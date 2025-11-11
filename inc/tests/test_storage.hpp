#ifndef INC_TESTS_TEST_STORAGE_HPP_
#define INC_TESTS_TEST_STORAGE_HPP_

#include <cassert>
#include <cmath>

#include "utils/storage.hpp"

void TestGetStorageModel();
void TestGetStorageType();
void TestGetStorageSizeGB();
void TestGetStorageUsage();
void TestGetMountpoints();
void TestReadAllStorageDevices();
void TestShowAllStorageDevices();
void TestGetVerboseStorageInfo();
void TestSaveConfidentialStorageInfo();

#endif /* INC_TESTS_TEST_STORAGE_HPP_ */
