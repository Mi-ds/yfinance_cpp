cmake_minimum_required(VERSION 3.15)

project(yfinance_cpp VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")

# Find required packages
find_package(PkgConfig REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Option to build tests
option(BUILD_TESTS "Build test programs" ON)

# Add subdirectories
add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
add_subdirectory(examples)

# Export targets for easy use in other projects
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*"
)

install(TARGETS yfinance_cpp
    EXPORT yfinance_cpp_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

export(EXPORT yfinance_cpp_targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/yfinance_cpp-config.cmake"
)