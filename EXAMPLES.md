# Example Usage

Here are some example programs demonstrating how to use the yfinance_cpp library:

## Basic Example

The `example_simple.cpp` shows basic usage:

```cpp
#include <iostream>
#include <string>

#include "yfconvert.h"
#include "ticker.h"

int main() {
    try {
        // Initialize the library
        yfconvert::initialize();
        
        // Create a ticker object for Apple
        yfconvert::Ticker apple("AAPL");
        
        std::cout << "Fetching information for ticker: " << apple.get_symbol() << std::endl;
        
        // Get basic info about the company
        auto info = apple.get_info();
        if (!info.is_null() && info.contains("longName")) {
            std::cout << "Company Name: " << info["longName"] << std::endl;
        } else if (!info.is_null() && info.contains("quoteType") && info["quoteType"].contains("longName")) {
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
        
        std::cout << "Example completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup the library
    yfconvert::cleanup();
    
    return 0;
}
```

## Compilation

To compile the example with the library:

```bash
g++ -std=c++17 example_simple.cpp -lyfinance_cpp -lcurl -o example_simple -L/path/to/lib -I/path/to/include
```

Or when linking statically:
```bash
g++ -std=c++17 example_simple.cpp -o example_simple -L/path/to/lib -I/path/to/include /path/to/lib/libyfinance_cpp.a -lcurl -lnlohmann_json
```

## More Examples

Additional examples include:
- `example_advanced.cpp` - More comprehensive usage
- `example_ko_mo_test.cpp` - Real-world stock data retrieval
- `example_endpoint_test.cpp` - Testing different API endpoints