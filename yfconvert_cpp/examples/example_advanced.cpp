#include <iostream>
#include <string>

#include "yfconvert.h"
#include "ticker.h"

int main() {
    try {
        // Initialize the library
        yfinance::initialize();
        
        // Create a ticker object for Apple
        yfinance::Ticker apple("AAPL");
        
        std::cout << "Fetching information for ticker: " << apple.get_symbol() << std::endl;
        
        // Get basic info about the company
        auto info = apple.get_info();
        if (!info.is_null() && info.contains("quoteType") && info["quoteType"].contains("longName")) {
            std::cout << "Company Name: " << info["quoteType"]["longName"] << std::endl;
        } else {
            std::cout << "Could not retrieve company name" << std::endl;
        }
        
        // Get historical data (last 30 days)
        auto history = apple.history(30, "1d");
        std::cout << "Historical data retrieved." << std::endl;
        
        // Get earnings
        auto earnings = apple.get_earnings();
        std::cout << "Earnings data retrieved." << std::endl;
        
        // Get financials
        auto financials = apple.get_financials();
        std::cout << "Financial data retrieved." << std::endl;
        
        std::cout << "Advanced example completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup the library
    yfinance::cleanup();
    
    return 0;
}