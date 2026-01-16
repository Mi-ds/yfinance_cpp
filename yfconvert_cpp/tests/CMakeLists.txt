# Find GoogleTest
find_package(GTest QUIET)
if(NOT GTEST_FOUND)
    message(WARNING "GoogleTest not found. Tests will not be built.")
else()
    enable_testing()

    # Define test source files
    set(TEST_SOURCES
        test_ticker.cpp
        test_http_client.cpp
        test_json_parser.cpp
        test_date_utils.cpp
    )

    # Create test executable
    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} ${test_src})
        target_link_libraries(${test_name} yfconvert_cpp GTest::gtest GTest::gtest_main GTest::gmock)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()