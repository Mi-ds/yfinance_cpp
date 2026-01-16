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
        
        std::cout << "Testing different endpoints for ticker: " << pep.get_symbol() << std::endl;
        
        // Test 1: Historical data (this worked before)
        try {
            auto history = pep.history(5, "1d");
            std::cout << "✓ Historical data endpoint: SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Historical data endpoint: FAILED - " << e.what() << std::endl;
        }
        
        // Test 2: Company info
        try {
            auto info = pep.get_info();
            std::cout << "✓ Info endpoint: SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Info endpoint: FAILED - " << e.what() << std::endl;
        }
        
        // Test 3: Earnings
        try {
            auto earnings = pep.get_earnings();
            std::cout << "✓ Earnings endpoint: SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Earnings endpoint: FAILED - " << e.what() << std::endl;
        }
        
        // Test 4: Financials
        try {
            auto financials = pep.get_financials();
            std::cout << "✓ Financials endpoint: SUCCESS" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Financials endpoint: FAILED - " << e.what() << std::endl;
        }
        
        std::cout << "Endpoint testing completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup the library
    yfinance::cleanup();
    
    return 0;
}