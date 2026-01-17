#ifndef YFINANCE_H
#define YFINANCE_H

#include "ticker.h"
#include "yf_data.h"
#include "http_client.h"
#include "utils.h"
#include "date_utils.h"

// Main yfinance namespace
namespace yfinance {

    // Version information
    constexpr const char* VERSION = "1.0.0";
    
    // Initialize the library
    void initialize();
    
    // Clean up resources
    void cleanup();

} // namespace yfinance

#endif // YFINANCE_H