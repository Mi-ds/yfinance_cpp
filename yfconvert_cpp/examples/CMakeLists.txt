# Example programs

# Simple example
add_executable(example_simple example_simple.cpp)
target_link_libraries(example_simple yfinance_cpp)

# Advanced example
add_executable(example_advanced example_advanced.cpp)
target_link_libraries(example_advanced yfinance_cpp)

# Basic example (no API calls)
add_executable(example_basic example_basic.cpp)
target_link_libraries(example_basic yfinance_cpp)

# Endpoint test example
add_executable(example_endpoint_test example_endpoint_test.cpp)
target_link_libraries(example_endpoint_test yfinance_cpp)

# KO and MO test example
add_executable(example_ko_mo_test example_ko_mo_test.cpp)
target_link_libraries(example_ko_mo_test yfinance_cpp)