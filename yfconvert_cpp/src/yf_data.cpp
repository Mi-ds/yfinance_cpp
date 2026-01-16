#include "yf_data.h"
#include "utils.h"
#include "http_client.h"
#include "json_parser.h"

#include <sstream>
#include <thread>
#include <regex>

namespace yfinance {

    YfData::YfData() : base_url_("https://query1.finance.yahoo.com"), proxy_(""), retries_(3) {
        http_client_ = std::make_unique<HttpClient>();
        http_client_->set_retries(retries_);
    }

    YfData::~YfData() = default;

    bool YfData::init_session() {
        // First, try to get the crumb token
        return get_crumb_token();
    }

    bool YfData::get_crumb_token() {
        try {
            // Step 1: Get initial cookie by visiting fc.yahoo.com (following Python yfinance approach)
            std::string cookie_url = "https://fc.yahoo.com";
            auto headers = Utils::get_default_headers();

            try {
                http_client_->get(cookie_url, headers);
                // Store cookies from this request for future use
                cookie_data_ = http_client_->get_cookies();
            } catch (const HttpClientException& e) {
                // If cookie request fails, we still continue as the crumb request might work
            }

            // Step 2: Get the crumb token from the dedicated endpoint
            std::string crumb_url = "https://query1.finance.yahoo.com/v1/test/getcrumb";

            // Include stored cookies in the request for the crumb
            auto crumb_headers = Utils::get_default_headers();
            if (!cookie_data_.empty()) {
                crumb_headers["Cookie"] = cookie_data_;
            }

            try {
                // Get the crumb as plain text (not JSON)
                std::string raw_response = http_client_->get_text(crumb_url, crumb_headers);

                // The response should be the crumb token as plain text
                if (!raw_response.empty() && raw_response.find("<html>") == std::string::npos) {
                    crumb_token_ = raw_response;
                } else {
                    // If we got HTML instead of a token, the session wasn't established properly
                    return false;
                }
            } catch (const HttpClientException& e) {
                // Crumb request might fail if we don't have proper session yet
                return false;
            }

            return !crumb_token_.empty();
        } catch (const std::exception& e) {
            return false;
        }
    }

    nlohmann::json YfData::get_raw_data(
        const std::string& symbol,
        const std::string& path,
        const std::map<std::string, std::string>& params
    ) {
        std::string url = base_url_ + path;

        // Add symbol to parameters if not already present
        auto all_params = params;
        if (all_params.find("symbol") == all_params.end()) {
            all_params["symbol"] = symbol;
        }

        // Add crumb token to parameters if we have one
        if (!crumb_token_.empty()) {
            all_params["crumb"] = crumb_token_;
        }

        // Set default headers
        auto headers = Utils::get_default_headers();

        // Add cookie header if we have cookie data
        if (!cookie_data_.empty()) {
            headers["Cookie"] = cookie_data_;
        }

        try {
            return http_client_->get(url, headers, all_params);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to fetch data for symbol " + symbol + ": " + e.what());
        }
    }

    nlohmann::json YfData::get_raw_data_with_session(
        const std::string& symbol,
        const std::string& path,
        const std::map<std::string, std::string>& params,
        int timeout
    ) {
        // Initialize session if not already done
        if (crumb_token_.empty()) {
            init_session();
        }

        std::string url = base_url_ + path;

        // Add symbol to parameters if not already present
        auto all_params = params;
        if (all_params.find("symbol") == all_params.end()) {
            all_params["symbol"] = symbol;
        }

        // Add crumb token to parameters if we have one
        if (!crumb_token_.empty()) {
            all_params["crumb"] = crumb_token_;
        }

        // Set headers and timeout
        auto headers = Utils::get_default_headers();

        // Add cookie header if we have cookie data
        if (!cookie_data_.empty()) {
            headers["Cookie"] = cookie_data_;
        }

        http_client_->set_timeout(timeout);

        try {
            return http_client_->get(url, headers, all_params);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to fetch data with session for symbol " + symbol + ": " + e.what());
        }
    }

    void YfData::clear_cache() {
        // In a more advanced implementation, we might have caching here
        // For now, this is a no-op
    }

    void YfData::set_proxy(const std::string& proxy) {
        proxy_ = proxy;
        http_client_->set_proxy(proxy);
    }

    void YfData::set_retries(int retries) {
        retries_ = retries;
        http_client_->set_retries(retries);
    }

} // namespace yfinance