cmake_minimum_required(VERSION 3.6)
project(observer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_COMPILER_LAUNCHER ccache)
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_subdirectory(external/FTXUI)

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
    src/ram.cpp
    src/storage.cpp
)

add_library(observer_lib STATIC ${PROJECT_SOURCES})
target_include_directories(observer_lib PUBLIC 
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/FTXUI/include
)

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


find_program(CLANG_FORMAT_EXECUTABLE clang-format)

if(CLANG_FORMAT_EXECUTABLE)
    message(STATUS "Found clang-format: ${CLANG_FORMAT_EXECUTABLE}")

    add_custom_target(
        format
        
        COMMAND ${CMAKE_COMMAND} -E echo "Running clang-format (style=file) on project sources..."
        
        COMMAND bash -c "find ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/inc -regex \".*\\.\\(cpp\\|hpp\\)$\" -exec ${CLANG_FORMAT_EXECUTABLE} -style=file -i {} \;"
        
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        
        COMMENT "Formatting C++ source files using clang-format."
        VERBATIM
    )

    message(STATUS "C++ formatting target 'format' created. Run 'ninja format' to format files.")

else()
    message(WARNING "clang-format executable not found. The 'format' target will not be available.")
endif()
