#include "date_utils.h"
#include <iomanip>
#include <sstream>

namespace yfinance {

    std::time_t DateUtils::string_to_timestamp(const std::string& date_str, const std::string& format) {
        std::tm tm = {};
        std::istringstream ss(date_str);
        ss >> std::get_time(&tm, format.c_str());
        
        if (ss.fail()) {
            throw std::invalid_argument("Unable to parse date string: " + date_str);
        }
        
        return std::mktime(&tm);
    }

    std::string DateUtils::timestamp_to_string(std::time_t timestamp, const std::string& format) {
        std::tm* tm = std::localtime(&timestamp);
        std::ostringstream oss;
        oss << std::put_time(tm, format.c_str());
        return oss.str();
    }

    std::time_t DateUtils::now() {
        return std::time(nullptr);
    }

    int DateUtils::days_between(const std::string& date1, const std::string& date2, const std::string& format) {
        std::tm tm1 = {}, tm2 = {};
        
        std::istringstream ss1(date1);
        ss1 >> std::get_time(&tm1, format.c_str());
        
        if (ss1.fail()) {
            throw std::invalid_argument("Unable to parse date string: " + date1);
        }
        
        std::istringstream ss2(date2);
        ss2 >> std::get_time(&tm2, format.c_str());
        
        if (ss2.fail()) {
            throw std::invalid_argument("Unable to parse date string: " + date2);
        }
        
        std::time_t time1 = std::mktime(&tm1);
        std::time_t time2 = std::mktime(&tm2);
        
        const int seconds_per_day = 60 * 60 * 24;
        return static_cast<int>(std::difftime(time2, time1) / seconds_per_day);
    }

    std::string DateUtils::format_date_range(std::time_t start, std::time_t end) {
        std::ostringstream oss;
        oss << timestamp_to_string(start, "%Y-%m-%d") << " to " << timestamp_to_string(end, "%Y-%m-%d");
        return oss.str();
    }

    bool DateUtils::is_valid_date(const std::string& date_str, const std::string& format) {
        std::tm tm = {};
        std::istringstream ss(date_str);
        ss >> std::get_time(&tm, format.c_str());
        
        return !ss.fail();
    }

    std::string DateUtils::add_days(const std::string& date_str, int days, const std::string& format) {
        std::tm tm = {};
        std::istringstream ss(date_str);
        ss >> std::get_time(&tm, format.c_str());
        
        if (ss.fail()) {
            throw std::invalid_argument("Unable to parse date string: " + date_str);
        }
        
        // Convert to time_t, add days, then convert back
        std::time_t time = std::mktime(&tm);
        time += days * 24 * 60 * 60;  // Add seconds for the specified number of days
        
        return timestamp_to_string(time, format);
    }

} // namespace yfinance