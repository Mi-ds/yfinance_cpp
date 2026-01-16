# yfinance_cppConfig.cmake
#
# This file helps CMake-based projects find the yfinance_cpp library.
# It sets the following variables:
#   yfinance_cpp_FOUND - true if the library was found
#   yfinance_cpp_INCLUDE_DIRS - directory containing header files
#   yfinance_cpp_LIBRARIES - libraries to link against

# Find the yfinance_cpp headers
find_path(yfinance_cpp_INCLUDE_DIR
    NAMES yfconvert.h
    PATHS ${CMAKE_CURRENT_LIST_DIR}/../../include
          /usr/local/include
          /usr/include
)

# Find the yfinance_cpp library
find_library(yfinance_cpp_LIBRARY
    NAMES yfinance_cpp
    PATHS ${CMAKE_CURRENT_LIST_DIR}/../../lib
          /usr/local/lib
          /usr/lib
)

# Set the version (you can adjust this)
set(yfinance_cpp_VERSION "1.0.0")

# Set the found status
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yfinance_cpp
    FOUND_VAR yfinance_cpp_FOUND
    REQUIRED_VARS yfinance_cpp_INCLUDE_DIR yfinance_cpp_LIBRARY
    VERSION_VAR yfinance_cpp_VERSION
)

# Create the imported target if it doesn't exist
if(yfinance_cpp_FOUND AND NOT TARGET yfinance_cpp::yfinance_cpp)
    add_library(yfinance_cpp::yfinance_cpp UNKNOWN IMPORTED)
    set_target_properties(yfinance_cpp::yfinance_cpp PROPERTIES
        IMPORTED_LOCATION "${yfinance_cpp_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${yfinance_cpp_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "curl"
    )
endif()

# Set legacy variables for compatibility
if(yfinance_cpp_FOUND)
    set(yfinance_cpp_INCLUDE_DIRS ${yfinance_cpp_INCLUDE_DIR})
    set(yfinance_cpp_LIBRARIES ${yfinance_cpp_LIBRARY})
endif()