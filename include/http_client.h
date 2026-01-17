#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <map>
#include <memory>

#include "json_parser.h"

namespace yfinance {

    // Custom exception for HTTP client errors
    class HttpClientException : public std::exception {
    public:
        explicit HttpClientException(const std::string& message) : msg_(message) {}
        virtual const char* what() const noexcept override { return msg_.c_str(); }
    private:
        std::string msg_;
    };

    /**
     * @brief HTTP client wrapper for making requests to Yahoo Finance API
     */
#ifdef USE_CPR
#include <cpr/cpr.h>
#elif defined(USE_CPP_HTTP_LIB)
#include <httplib.h>
#else
// Fallback to libcurl C++ wrapper
#include <curl/curl.h>
#endif

    class HttpClient {
    public:
        HttpClient();
        ~HttpClient();

        // GET request returning JSON
        nlohmann::json get(const std::string& url,
                          const std::map<std::string, std::string>& headers = {},
                          const std::map<std::string, std::string>& params = {});

        // GET request returning raw text
        std::string get_text(const std::string& url,
                            const std::map<std::string, std::string>& headers = {},
                            const std::map<std::string, std::string>& params = {});

        // POST request
        nlohmann::json post(const std::string& url,
                           const std::string& data,
                           const std::map<std::string, std::string>& headers = {});

        // Set proxy
        void set_proxy(const std::string& proxy);

        // Set number of retries for failed requests
        void set_retries(int retries);

        // Set timeout for requests
        void set_timeout(int seconds);

        // Set user agent string
        void set_user_agent(const std::string& user_agent);

        // Get cookie data
        std::string get_cookies() const { return cookies_; }

        // Set cookie data
        void set_cookies(const std::string& cookies) { cookies_ = cookies; }

    private:
        std::string proxy_;
        std::string user_agent_;
        std::string cookies_;
        int retries_;
        int timeout_;

#ifndef USE_CPR
#ifndef USE_CPP_HTTP_LIB
        CURL* curl_handle_;
#endif
#endif

        // Perform request with retry logic
        std::string perform_request(const std::string& method,
                                   const std::string& url,
                                   const std::map<std::string, std::string>& headers,
                                   const std::string& data = "");

        // Check if error is transient and should be retried
        bool is_transient_error(const std::string& error_message);

#ifndef USE_CPR
#ifndef USE_CPP_HTTP_LIB
        // Static callback for libcurl
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp);
#endif
#endif
    };

} // namespace yfinance

#endif // HTTP_CLIENT_H