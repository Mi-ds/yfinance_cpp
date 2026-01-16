#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>
#include <chrono>
#include <ctime>

namespace yfinance {

    /**
     * @brief Utilities for date and time manipulation
     */
    class DateUtils {
    public:
        // Convert string date to timestamp
        static std::time_t string_to_timestamp(const std::string& date_str, 
                                              const std::string& format = "%Y-%m-%d");

        // Convert timestamp to string date
        static std::string timestamp_to_string(std::time_t timestamp, 
                                              const std::string& format = "%Y-%m-%d");

        // Get current timestamp
        static std::time_t now();

        // Calculate days between two dates
        static int days_between(const std::string& date1, const std::string& date2,
                               const std::string& format = "%Y-%m-%d");

        // Format date range for API calls
        static std::string format_date_range(std::time_t start, std::time_t end);

        // Validate date string format
        static bool is_valid_date(const std::string& date_str, 
                                 const std::string& format = "%Y-%m-%d");

        // Add days to a date
        static std::string add_days(const std::string& date_str, int days,
                                   const std::string& format = "%Y-%m-%d");
    };

} // namespace yfinance

#endif // DATE_UTILS_H