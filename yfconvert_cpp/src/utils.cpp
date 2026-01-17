#include "utils.h"
#include <cctype>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>

namespace yfinance {

    std::string Utils::url_encode(const std::string& str) {
        std::string encoded;
        encoded.reserve(str.length());
        
        for (unsigned char c : str) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded += c;
            } else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", c);
                encoded += buf;
            }
        }
        
        return encoded;
    }

    std::string Utils::format_error(const std::string& msg) {
        return "[ERROR] " + msg;
    }

    void Utils::sleep_ms(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    std::string Utils::random_string(size_t length) {
        const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, charset.size() - 1);
        
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            result += charset[dis(gen)];
        }
        
        return result;
    }

    std::vector<std::string> Utils::split_string(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }

    std::string Utils::trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    bool Utils::starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    bool Utils::ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    std::string Utils::to_lowercase(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower;
    }

    std::string Utils::replace_all(std::string str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        return str;
    }

    bool Utils::is_valid_ticker(const std::string& symbol) {
        // Basic validation: 1-6 alphanumeric characters and dots/hyphens
        if (symbol.empty() || symbol.length() > 10) {
            return false;
        }
        
        for (char c : symbol) {
            if (!std::isalnum(c) && c != '.' && c != '-') {
                return false;
            }
        }
        
        return true;
    }

    std::string Utils::days_to_period_string(int days) {
        // Map days to appropriate period strings that Yahoo Finance API recognizes
        if (days <= 1) return "1d";
        if (days <= 5) return "5d";
        if (days <= 30) return "1mo";
        if (days <= 90) return "3mo";
        if (days <= 180) return "6mo";
        if (days <= 365) return "1y";
        if (days <= 730) return "2y";  // Approx 2 years
        if (days <= 1825) return "5y"; // Approx 5 years
        if (days <= 3650) return "10y"; // Approx 10 years
        return "max"; // For periods longer than 10 years, use max
    }

    std::map<std::string, std::string> Utils::get_default_headers() {
        std::map<std::string, std::string> headers;
        headers["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";
        headers["Accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8,application/json";
        headers["Accept-Language"] = "en-US,en;q=0.9";
        headers["Accept-Encoding"] = "gzip, deflate, br";
        headers["Connection"] = "keep-alive";
        headers["Upgrade-Insecure-Requests"] = "1";
        headers["Sec-Fetch-Dest"] = "document";
        headers["Sec-Fetch-Mode"] = "navigate";
        headers["Sec-Fetch-Site"] = "none";
        headers["Cache-Control"] = "max-age=0";

        return headers;
    }

} // namespace yfinance