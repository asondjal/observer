cmake_minimum_required(VERSION 3.6)
project(observer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")

add_subdirectory(external/ftxui)

add_executable(observer
    src/main.cpp
)

target_link_libraries(observer PRIVATE observer_lib gcov)

target_link_libraries(observer PRIVATE observer_lib)
target_compile_options(observer PRIVATE -Wall -Wextra -pedantic)

set(PROJECT_SOURCES
    src/read_data.cpp
    src/logging.cpp
    src/cpu.cpp
    src/utilities.cpp
    src/monitoring.cpp
)

add_library(observer_lib STATIC ${PROJECT_SOURCES})
target_include_directories(observer_lib PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_compile_options(observer_lib PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(observer_lib PUBLIC
    ftxui::screen    
    ftxui::dom           
    ftxui::component
    poppler-cpp
    tinyxml2
    poppler-cpp
    tinyxml2
    ${STATGRAB_LIB}
    ${SENSORS_LIB}
    ${CPUID_LIB}
    ${NL_LIB}
    ${NL_GENL_LIB}
)

add_executable(cpp_tests
    src/read_data.cpp
    src/logging.cpp
    src/utilities.cpp

    cpp_tests/main.cpp
    cpp_tests/test_utilities.cpp
    cpp_tests/test_read_data.cpp
    cpp_tests/test_logging.cpp
    cpp_tests/test_cpu.cpp
)

target_include_directories(cpp_tests PRIVATE ${PROJECT_SOURCE_DIR}/inc)

target_compile_options(cpp_tests PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(cpp_tests PRIVATE
    observer_lib
    gcov
)