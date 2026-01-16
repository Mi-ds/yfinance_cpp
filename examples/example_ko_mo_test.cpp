#include <iostream>
#include <string>

#include "yfconvert.h"
#include "ticker.h"

int main() {
    try {
        // Initialize the library
        yfinance::initialize();
        
        std::cout << "=== Testing with Coca-Cola (KO) ===" << std::endl;
        
        // Create a ticker object for Coca-Cola
        yfinance::Ticker ko("KO");
        std::cout << "Symbol: " << ko.get_symbol() << std::endl;
        
        // Get company info
        auto info = ko.get_info();
        if (!info.is_null() && info.contains("longName")) {
            std::cout << "Company Name: " << info["longName"] << std::endl;
        } else if (!info.is_null() && info.contains("quoteType") && info["quoteType"].contains("longName")) {
            std::cout << "Company Name: " << info["quoteType"]["longName"] << std::endl;
        } else {
            std::cout << "Company Name: Not found" << std::endl;
        }
        
        if (!info.is_null() && info.contains("currentPrice")) {
            std::cout << "Current Price: $" << info["currentPrice"] << std::endl;
        } else if (!info.is_null() && info.contains("summaryDetail") && info["summaryDetail"].contains("previousClose")) {
            std::cout << "Previous Close: $" << info["summaryDetail"]["previousClose"] << std::endl;
        } else {
            std::cout << "Current/Previous Close: Not found" << std::endl;
        }
        
        // Get historical data
        auto history = ko.history(5, "1d");
        std::cout << "Historical data retrieved successfully!" << std::endl;
        if (history.contains("chart") && history["chart"].contains("result") && 
            !history["chart"]["result"].empty()) {
            auto result = history["chart"]["result"][0];
            if (result.contains("indicators") && result["indicators"].contains("quote") &&
                !result["indicators"]["quote"].empty()) {
                auto quotes = result["indicators"]["quote"][0];
                if (quotes.contains("close") && !quotes["close"].empty()) {
                    std::cout << "Latest closing prices: ";
                    // Print last 3 closing prices
                    int count = 0;
                    for (auto it = quotes["close"].rbegin(); it != quotes["close"].rend() && count < 3; ++it, ++count) {
                        std::cout << "$" << *it << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        std::cout << std::endl << "=== Testing with Altria (MO) ===" << std::endl;
        
        // Create a ticker object for Altria
        yfinance::Ticker mo("MO");
        std::cout << "Symbol: " << mo.get_symbol() << std::endl;
        
        // Get company info
        auto info2 = mo.get_info();
        if (!info2.is_null() && info2.contains("longName")) {
            std::cout << "Company Name: " << info2["longName"] << std::endl;
        } else if (!info2.is_null() && info2.contains("quoteType") && info2["quoteType"].contains("longName")) {
            std::cout << "Company Name: " << info2["quoteType"]["longName"] << std::endl;
        } else {
            std::cout << "Company Name: Not found" << std::endl;
        }
        
        if (!info2.is_null() && info2.contains("currentPrice")) {
            std::cout << "Current Price: $" << info2["currentPrice"] << std::endl;
        } else if (!info2.is_null() && info2.contains("summaryDetail") && info2["summaryDetail"].contains("previousClose")) {
            std::cout << "Previous Close: $" << info2["summaryDetail"]["previousClose"] << std::endl;
        } else {
            std::cout << "Current/Previous Close: Not found" << std::endl;
        }
        
        // Get historical data
        auto history2 = mo.history(5, "1d");
        std::cout << "Historical data retrieved successfully!" << std::endl;
        if (history2.contains("chart") && history2["chart"].contains("result") && 
            !history2["chart"]["result"].empty()) {
            auto result = history2["chart"]["result"][0];
            if (result.contains("indicators") && result["indicators"].contains("quote") &&
                !result["indicators"]["quote"].empty()) {
                auto quotes = result["indicators"]["quote"][0];
                if (quotes.contains("close") && !quotes["close"].empty()) {
                    std::cout << "Latest closing prices: ";
                    // Print last 3 closing prices
                    int count = 0;
                    for (auto it = quotes["close"].rbegin(); it != quotes["close"].rend() && count < 3; ++it, ++count) {
                        std::cout << "$" << *it << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        std::cout << std::endl << "Both stocks tested successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup the library
    yfinance::cleanup();
    
    return 0;
}