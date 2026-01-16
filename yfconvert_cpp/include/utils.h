#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <map>

#include "json_parser.h"

namespace yfinance {

    /**
     * @brief General utility functions
     */
    class Utils {
    public:
        // URL encode a string
        static std::string url_encode(const std::string& str);

        // Format error messages
        static std::string format_error(const std::string& msg);

        // Sleep for specified milliseconds
        static void sleep_ms(int milliseconds);

        // Generate random string
        static std::string random_string(size_t length);

        // Split string by delimiter
        static std::vector<std::string> split_string(const std::string& str, char delimiter);

        // Trim whitespace from string
        static std::string trim(const std::string& str);

        // Check if string starts with prefix
        static bool starts_with(const std::string& str, const std::string& prefix);

        // Check if string ends with suffix
        static bool ends_with(const std::string& str, const std::string& suffix);

        // Convert string to lowercase
        static std::string to_lowercase(const std::string& str);

        // Replace substring in string
        static std::string replace_all(std::string str, 
                                      const std::string& from, 
                                      const std::string& to);

        // Validate ticker symbol format
        static bool is_valid_ticker(const std::string& symbol);

        // Get default headers for requests
        static std::map<std::string, std::string> get_default_headers();
    };

} // namespace yfinance

#endif // UTILS_H