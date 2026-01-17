#include <iostream>
#include <cassert>

#include "ticker.h"
#include "utils.h"

void test_url_encoding() {
    std::string input = "AAPL&info";
    std::string expected = "AAPL%26info";
    std::string result = yfconvert::Utils::url_encode(input);
    
    assert(result == expected);
    std::cout << "URL encoding test passed!" << std::endl;
}

void test_ticker_creation() {
    try {
        yfconvert::Ticker ticker("AAPL");
        assert(ticker.get_symbol() == "AAPL");
        std::cout << "Ticker creation test passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Ticker creation test failed: " << e.what() << std::endl;
    }
}

void test_invalid_ticker() {
    try {
        yfconvert::Ticker invalid_ticker("INVALID_TICKER_SYMBOL_TEST");
        // This should not be reached if validation works
        std::cout << "Invalid ticker test failed - should have thrown an exception" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Invalid ticker test passed - correctly threw exception: " << e.what() << std::endl;
    }
}

void test_date_utils() {
    try {
        // Test date parsing
        std::time_t ts = yfconvert::DateUtils::string_to_timestamp("2022-01-01", "%Y-%m-%d");
        std::string date_str = yfconvert::DateUtils::timestamp_to_string(ts, "%Y-%m-%d");
        assert(date_str == "2022-01-01");
        std::cout << "Date utilities test passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Date utilities test failed: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "Running unit tests..." << std::endl;
    
    test_url_encoding();
    test_ticker_creation();
    test_invalid_ticker();
    test_date_utils();
    
    std::cout << "All tests completed!" << std::endl;
    return 0;
}