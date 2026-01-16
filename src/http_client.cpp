#include "http_client.h"
#include "utils.h"
#include "json_parser.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <regex>

#ifdef USE_CPR
#include <cpr/cpr.h>
#elif defined(USE_CPP_HTTP_LIB)
#include <httplib.h>
#else
// Fallback to libcurl C++ wrapper
#include <curl/curl.h>
#endif

namespace yfinance {

    HttpClient::HttpClient() : proxy_(""), user_agent_("Mozilla/5.0 (compatible; yfinance-cpp/1.0)"), cookies_(""), retries_(3), timeout_(30) {
#ifdef USE_CPR
        // CPR initialization if needed
#elif defined(USE_CPP_HTTP_LIB)
        // cpp-httplib doesn't need special initialization
#else
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_handle_ = curl_easy_init();
        if (curl_handle_) {
            // Enable cookie handling
            curl_easy_setopt(curl_handle_, CURLOPT_COOKIEFILE, ""); // Initialize cookie engine
        }
#endif
    }

    HttpClient::~HttpClient() {
#ifdef USE_CPR
        // CPR cleanup if needed
#elif defined(USE_CPP_HTTP_LIB)
        // No cleanup needed for cpp-httplib
#else
        if (curl_handle_) {
            curl_easy_cleanup(curl_handle_);
            curl_handle_ = nullptr;
        }
        curl_global_cleanup();
#endif
    }

    nlohmann::json HttpClient::get(const std::string& url,
                                  const std::map<std::string, std::string>& headers,
                                  const std::map<std::string, std::string>& params) {
        std::string response = get_text(url, headers, params);

        if (response.empty()) {
            throw HttpClientException("Empty response from server for URL: " + url);
        }

        try {
            return JsonParser::parse(response);
        } catch (const std::exception& e) {
            throw HttpClientException("Failed to parse JSON response from " + url + ": " + std::string(e.what()));
        }
    }

    std::string HttpClient::get_text(const std::string& url,
                                   const std::map<std::string, std::string>& headers,
                                   const std::map<std::string, std::string>& params) {
        // Build query string from params
        std::string query_string = "";
        for (const auto& param : params) {
            if (!query_string.empty()) {
                query_string += "&";
            }
            query_string += Utils::url_encode(param.first) + "=" + Utils::url_encode(param.second);
        }

        std::string full_url = url;
        if (!query_string.empty()) {
            full_url += "?" + query_string;
        }

        return perform_request("GET", full_url, headers, "");
    }

    nlohmann::json HttpClient::post(const std::string& url,
                                   const std::string& data,
                                   const std::map<std::string, std::string>& headers) {
        std::string response = perform_request("POST", url, headers, data);

        if (response.empty()) {
            throw HttpClientException("Empty response from server for URL: " + url);
        }

        try {
            return JsonParser::parse(response);
        } catch (const std::exception& e) {
            throw HttpClientException("Failed to parse JSON response from " + url + ": " + std::string(e.what()));
        }
    }

    void HttpClient::set_proxy(const std::string& proxy) {
        proxy_ = proxy;
    }

    void HttpClient::set_retries(int retries) {
        retries_ = retries;
    }

    void HttpClient::set_timeout(int seconds) {
        timeout_ = seconds;
    }

    void HttpClient::set_user_agent(const std::string& user_agent) {
        user_agent_ = user_agent;
    }

    bool HttpClient::is_transient_error(const std::string& error_message) {
        std::string lower_error = Utils::to_lowercase(error_message);

        // Check for common transient error patterns
        std::vector<std::string> transient_patterns = {
            "timeout", "connection refused", "connection reset", "network is unreachable",
            "temporary failure in name resolution", "too many redirects", "could not resolve host"
        };

        for (const auto& pattern : transient_patterns) {
            if (lower_error.find(pattern) != std::string::npos) {
                return true;
            }
        }

        return false;
    }

    std::string HttpClient::perform_request(const std::string& method,
                                           const std::string& url,
                                           const std::map<std::string, std::string>& headers,
                                           const std::string& data) {
        // Add user-agent to headers if not already present
        auto request_headers = headers;
        if (request_headers.find("User-Agent") == request_headers.end()) {
            request_headers["User-Agent"] = user_agent_;
        }

        // Retry mechanism
        for (int attempt = 0; attempt <= retries_; ++attempt) {
            try {
#ifdef USE_CPR
                cpr::Header cpr_headers;
                for (const auto& header : request_headers) {
                    cpr_headers.insert({header.first, header.second});
                }

                cpr::Parameters cpr_params; // For parameters if needed

                if (method == "GET") {
                    cpr::Response response = cpr::Get(
                        cpr::Url{url},
                        cpr_headers,
                        cpr::Timeout{timeout_ * 1000}
                    );

                    if (response.error.code != cpr::ErrorCode::OK) {
                        std::string error_msg = response.error.message;
                        if (attempt < retries_ && is_transient_error(error_msg)) {
                            Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                            continue;
                        } else {
                            throw HttpClientException("Request failed: " + error_msg);
                        }
                    }

                    // Check for HTTP error status codes
                    if (response.status_code >= 400) {
                        throw HttpClientException("HTTP error " + std::to_string(response.status_code) +
                                                 " for URL: " + url);
                    }

                    return response.text;
                } else if (method == "POST") {
                    cpr::Response response = cpr::Post(
                        cpr::Url{url},
                        cpr::Body{data},
                        cpr_headers,
                        cpr::Timeout{timeout_ * 1000}
                    );

                    if (response.error.code != cpr::ErrorCode::OK) {
                        std::string error_msg = response.error.message;
                        if (attempt < retries_ && is_transient_error(error_msg)) {
                            Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                            continue;
                        } else {
                            throw HttpClientException("Request failed: " + error_msg);
                        }
                    }

                    // Check for HTTP error status codes
                    if (response.status_code >= 400) {
                        throw HttpClientException("HTTP error " + std::to_string(response.status_code) +
                                                 " for URL: " + url);
                    }

                    return response.text;
                }

#elif defined(USE_CPP_HTTP_LIB)
                httplib::Client client(url);

                // Set timeout
                client.set_connection_timeout(timeout_);
                client.set_read_timeout(timeout_);
                client.set_write_timeout(timeout_);

                if (!proxy_.empty()) {
                    client.set_proxy(proxy_);
                }

                httplib::Headers httplib_headers;
                for (const auto& header : request_headers) {
                    httplib_headers.insert(header);
                }

                if (method == "GET") {
                    auto response = client.Get(url.c_str(), httplib_headers);
                    if (response) {
                        if (response->status >= 200 && response->status < 300) {
                            return response->body;
                        } else if (response->status >= 400 && response->status < 600) {
                            // Handle HTTP error codes
                            if (attempt < retries_) {
                                Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                                continue;
                            } else {
                                throw HttpClientException("HTTP error " + std::to_string(response->status) +
                                                         " for URL: " + url);
                            }
                        } else {
                            // Unexpected status code
                            if (attempt < retries_) {
                                Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                                continue;
                            } else {
                                throw HttpClientException("Unexpected status code " + std::to_string(response->status) +
                                                         " for URL: " + url);
                            }
                        }
                    } else {
                        std::string error_msg = "No response received";
                        if (attempt < retries_ && is_transient_error(error_msg)) {
                            Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                            continue;
                        } else {
                            throw HttpClientException("Request failed: " + error_msg);
                        }
                    }
                } else if (method == "POST") {
                    auto response = client.Post(url.c_str(), httplib_headers, data, "application/json");
                    if (response) {
                        if (response->status >= 200 && response->status < 300) {
                            return response->body;
                        } else if (response->status >= 400 && response->status < 600) {
                            // Handle HTTP error codes
                            if (attempt < retries_) {
                                Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                                continue;
                            } else {
                                throw HttpClientException("HTTP error " + std::to_string(response->status) +
                                                         " for URL: " + url);
                            }
                        } else {
                            // Unexpected status code
                            if (attempt < retries_) {
                                Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                                continue;
                            } else {
                                throw HttpClientException("Unexpected status code " + std::to_string(response->status) +
                                                         " for URL: " + url);
                            }
                        }
                    } else {
                        std::string error_msg = "No response received";
                        if (attempt < retries_ && is_transient_error(error_msg)) {
                            Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                            continue;
                        } else {
                            throw HttpClientException("Request failed: " + error_msg);
                        }
                    }
                }
#else
                // Using the persistent libcurl handle for cookie/session management
                if (!curl_handle_) {
                    throw HttpClientException("CURL handle not initialized");
                }

                CURLcode res;
                std::string read_buffer;

                // Reset the handle to clear previous settings
                curl_easy_reset(curl_handle_);

                curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());

                if (method == "POST") {
                    curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, data.c_str());
                }

                struct curl_slist *chunk = NULL;
                for (const auto& header : request_headers) {
                    std::string header_str = header.first + ": " + header.second;
                    chunk = curl_slist_append(chunk, header_str.c_str());
                }
                if (chunk) {
                    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, chunk);
                }

                if (!proxy_.empty()) {
                    curl_easy_setopt(curl_handle_, CURLOPT_PROXY, proxy_.c_str());
                }

                curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, timeout_);

                // Enable automatic decompression
                curl_easy_setopt(curl_handle_, CURLOPT_ACCEPT_ENCODING, "");

                // Use the cookie engine that was initialized in constructor
                curl_easy_setopt(curl_handle_, CURLOPT_COOKIEFILE, ""); // Enable cookies

                // For reading response
                curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, WriteCallback);
                curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &read_buffer);

                // Follow redirects as the Python version does
                curl_easy_setopt(curl_handle_, CURLOPT_FOLLOWLOCATION, 1L);

                res = curl_easy_perform(curl_handle_);

                long response_code;
                curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &response_code);

                // Get cookies after the request to update our cookie store
                struct curl_slist *cookies = NULL;
                res = curl_easy_getinfo(curl_handle_, CURLINFO_COOKIELIST, &cookies);
                if (res == CURLE_OK && cookies) {
                    struct curl_slist *current = cookies;
                    std::string all_cookies = "";

                    while (current) {
                        // Extract just the cookie name=value part
                        std::string cookie_line = std::string(current->data);
                        size_t tab_pos = cookie_line.rfind('\t');
                        if (tab_pos != std::string::npos) {
                            std::string cookie_part = cookie_line.substr(tab_pos + 1);
                            size_t eq_pos = cookie_part.find('=');
                            if (eq_pos != std::string::npos) {
                                if (!cookies_.empty()) {
                                    cookies_ += "; ";
                                }
                                cookies_ += cookie_part;
                            }
                        }
                        current = current->next;
                    }
                    curl_slist_free_all(cookies);
                }

                if (chunk) {
                    curl_slist_free_all(chunk);
                }

                if (res != CURLE_OK) {
                    std::string error_msg = curl_easy_strerror(res);
                    if (attempt < retries_ && is_transient_error(error_msg)) {
                        Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                        continue;
                    } else {
                        throw HttpClientException("Request failed: " + error_msg);
                    }
                }

                // Check HTTP response code
                if (response_code >= 400) {
                    if (attempt < retries_) {
                        Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                        continue;
                    } else {
                        throw HttpClientException("HTTP error " + std::to_string(response_code) +
                                                 " for URL: " + url);
                    }
                }

                return read_buffer;
#endif
            } catch (const HttpClientException& e) {
                // Don't retry for specific HTTP errors (like 404, 401, etc.)
                throw; // Re-throw HTTP client exceptions immediately
            } catch (const std::exception& e) {
                std::string error_msg = e.what();
                if (attempt < retries_ && is_transient_error(error_msg)) {
                    Utils::sleep_ms(1000 * (attempt + 1)); // Exponential backoff
                    continue;
                } else {
                    throw HttpClientException("Request failed: " + error_msg);
                }
            }
        }

        throw HttpClientException("Max retries exceeded for URL: " + url);
    }

#ifdef USE_CPR
    // No additional functions needed for CPR
#elif defined(USE_CPP_HTTP_LIB)
    // No additional functions needed for cpp-httplib
#else
    // Callback function for libcurl
    size_t HttpClient::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
        size_t totalSize = size * nmemb;
        userp->append((char*)contents, totalSize);
        return totalSize;
    }
#endif

} // namespace yfinance