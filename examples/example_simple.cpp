#include <iostream>
#include <string>

#include "yfconvert.h"
#include "ticker.h"

int main() {
    try {
        // Initialize the library
        yfinance::initialize();

        // Create a ticker object for PepsiCo
        yfinance::Ticker pep("PEP");

        std::cout << "Fetching information for ticker: " << pep.get_symbol() << std::endl;

        // Try getting historical data first (this endpoint may be more accessible)
        try {
            auto history = pep.history(5, "1d");
            std::cout << "Historical data retrieved successfully!" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Historical data retrieval failed: " << e.what() << std::endl;
        }

        // Then try getting basic info about the company
        try {
            auto info = pep.get_info();
            if (!info.is_null() && info.contains("quoteType") && info["quoteType"].contains("longName")) {
                std::cout << "Company Name: " << info["quoteType"]["longName"] << std::endl;
            } else {
                std::cout << "Could not retrieve company name" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Info retrieval failed: " << e.what() << std::endl;
        }

        std::cout << "Example completed with error handling!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Cleanup the library
    yfinance::cleanup();

    return 0;
}