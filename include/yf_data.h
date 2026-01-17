#ifndef YF_DATA_H
#define YF_DATA_H

#include <string>
#include <memory>
#include <map>

#include "http_client.h"
#include "json_parser.h"

namespace yfinance {

    /**
     * @brief Data provider class that handles communication with Yahoo Finance API
     */
    class YfData {
    public:
        YfData();
        ~YfData();

        // Initialize session with crumb token
        bool init_session();

        // Fetch data from Yahoo Finance API
        nlohmann::json get_raw_data(
            const std::string& symbol,
            const std::string& path,
            const std::map<std::string, std::string>& params = {}
        );

        // Fetch data with session management
        nlohmann::json get_raw_data_with_session(
            const std::string& symbol,
            const std::string& path,
            const std::map<std::string, std::string>& params = {},
            int timeout = 30
        );

        // Cache management
        void clear_cache();

        // Set proxy for HTTP requests
        void set_proxy(const std::string& proxy);

        // Set number of retries for failed requests
        void set_retries(int retries);

    private:
        std::unique_ptr<HttpClient> http_client_;
        std::string base_url_;
        std::string proxy_;
        int retries_;
        std::string crumb_token_;
        std::string cookie_data_;

        // Get crumb token for authenticated requests
        bool get_crumb_token();
    };

} // namespace yfinance

#endif // YF_DATA_H