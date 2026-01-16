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
        
        std::cout << "Successfully created Ticker object for: " << pep.get_symbol() << std::endl;
        
        // Just test that the object was created properly
        // We won't call the API methods to avoid HTTP errors
        std::cout << "Ticker object created successfully!" << std::endl;
        
        std::cout << "Basic functionality test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup the library
    yfinance::cleanup();
    
    return 0;
}