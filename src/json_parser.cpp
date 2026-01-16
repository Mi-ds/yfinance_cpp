#include "json_parser.h"
#include <sstream>
#include <stdexcept>

namespace yfinance {

    nlohmann::json JsonParser::parse(const std::string& json_str) {
        try {
            return nlohmann::json::parse(json_str);
        } catch (const std::exception& e) {
            throw std::runtime_error("JSON parse error: " + std::string(e.what()));
        }
    }

    std::string JsonParser::stringify(const nlohmann::json& obj, bool pretty) {
        try {
            if (pretty) {
                return obj.dump(4); // Pretty print with 4-space indent
            } else {
                return obj.dump(); // Compact format
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("JSON stringify error: " + std::string(e.what()));
        }
    }

    nlohmann::json JsonParser::extract_field(const nlohmann::json& obj, const std::string& field) {
        if (obj.contains(field)) {
            return obj.at(field);
        } else {
            throw std::runtime_error("Field '" + field + "' not found in JSON object");
        }
    }

    bool JsonParser::has_field(const nlohmann::json& obj, const std::string& field) {
        return obj.contains(field);
    }

    nlohmann::json JsonParser::merge(const nlohmann::json& obj1, const nlohmann::json& obj2) {
        nlohmann::json result = obj1;
        
        for (auto it = obj2.begin(); it != obj2.end(); ++it) {
            result[it.key()] = it.value();
        }
        
        return result;
    }

    nlohmann::json JsonParser::get_array_element(const nlohmann::json& arr, size_t index) {
        if (!arr.is_array()) {
            throw std::runtime_error("Object is not an array");
        }
        
        if (index >= arr.size()) {
            throw std::runtime_error("Index out of bounds for array");
        }
        
        return arr[index];
    }

    std::vector<std::string> JsonParser::get_keys(const nlohmann::json& obj) {
        if (!obj.is_object()) {
            throw std::runtime_error("Object is not a JSON object");
        }
        
        std::vector<std::string> keys;
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            keys.push_back(it.key());
        }
        
        return keys;
    }

} // namespace yfinance