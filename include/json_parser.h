#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>

// Import nlohmann json namespace
namespace json = nlohmann;

namespace yfinance {

    /**
     * @brief Utility class for JSON parsing and manipulation
     */
    class JsonParser {
    public:
        // Parse JSON string
        static json::json parse(const std::string& json_str);

        // Convert JSON object to string
        static std::string stringify(const json::json& obj, bool pretty = false);

        // Extract specific fields from JSON
        static json::json extract_field(const json::json& obj, const std::string& field);

        // Check if field exists in JSON
        static bool has_field(const json::json& obj, const std::string& field);

        // Merge two JSON objects
        static json::json merge(const json::json& obj1, const json::json& obj2);

        // Extract array element by index
        static json::json get_array_element(const json::json& arr, size_t index);

        // Get all keys from JSON object
        static std::vector<std::string> get_keys(const json::json& obj);
    };

} // namespace yfinance

#endif // JSON_PARSER_H