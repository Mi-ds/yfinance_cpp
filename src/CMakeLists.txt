# Define library source files
set(YFCONVERT_CPP_SOURCES
    yf_data.cpp
    ticker.cpp
    utils.cpp
    http_client.cpp
    date_utils.cpp
    json_parser.cpp
    data_structures.cpp
    yfconvert.cpp
)

# Define library headers
set(YFCONVERT_CPP_HEADERS
    ${PROJECT_SOURCE_DIR}/include/yf_data.h
    ${PROJECT_SOURCE_DIR}/include/ticker.h
    ${PROJECT_SOURCE_DIR}/include/utils.h
    ${PROJECT_SOURCE_DIR}/include/http_client.h
    ${PROJECT_SOURCE_DIR}/include/date_utils.h
    ${PROJECT_SOURCE_DIR}/include/json_parser.h
)

# Create both static and shared libraries
add_library(yfinance_cpp_static STATIC ${YFCONVERT_CPP_SOURCES} ${YFCONVERT_CPP_HEADERS})
add_library(yfinance_cpp SHARED ${YFCONVERT_CPP_SOURCES} ${YFCONVERT_CPP_HEADERS})

# Rename the static library to match the original name
set_target_properties(yfinance_cpp_static PROPERTIES OUTPUT_NAME yfinance_cpp)

# Link required libraries for both static and shared
target_link_libraries(yfinance_cpp_static
    ${CMAKE_DL_LIBS}
)

target_link_libraries(yfinance_cpp
    ${CMAKE_DL_LIBS}
)

# Set include directories for the libraries
target_include_directories(yfinance_cpp_static PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/../externals/include
)

target_include_directories(yfinance_cpp PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_BINARY_DIR}/../externals/include
)

# Find and link external libraries
find_package(PkgConfig REQUIRED)

# Check for nlohmann_json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(WARNING "nlohmann_json not found. You may need to install it.")
endif()

# Check for cpr (HTTP client)
find_package(cpr QUIET)
if(NOT cpr_FOUND)
    message(WARNING "cpr (HTTP client library) not found. You may need to install it.")
    # If cpr not found, we'll still build but will need libcurl linked externally
    message(WARNING "Building with libcurl support - ensure it's installed on the system")
endif()

if(nlohmann_json_FOUND)
    target_link_libraries(yfinance_cpp_static nlohmann_json::nlohmann_json)
    target_link_libraries(yfinance_cpp nlohmann_json::nlohmann_json)
endif()

if(cpr_FOUND)
    target_link_libraries(yfinance_cpp_static cpr::cpr)
    target_link_libraries(yfinance_cpp cpr::cpr)
else()
    # Link against system libcurl when cpr is not available
    target_link_libraries(yfinance_cpp_static curl)
    target_link_libraries(yfinance_cpp curl)
endif()