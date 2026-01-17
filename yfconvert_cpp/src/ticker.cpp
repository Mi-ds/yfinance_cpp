#include "ticker.h"
#include "yf_data.h"
#include "utils.h"
#include "date_utils.h"

#include <stdexcept>
#include <algorithm>

namespace yfinance {

    Ticker::Ticker(const std::string& symbol) : symbol_(symbol) {
        // Validate the symbol format
        if (!Utils::is_valid_ticker(symbol_)) {
            throw std::invalid_argument("Invalid ticker symbol: " + symbol_);
        }

        // Initialize the data provider
        data_provider_ = std::make_unique<YfData>();

        // Initialize session to get crumb token early
        data_provider_->init_session();
    }

    Ticker::~Ticker() = default;

    std::string Ticker::get_symbol() const {
        return symbol_;
    }

    nlohmann::json Ticker::history(
        int period_days,
        const std::string& interval,
        bool auto_adjust,
        bool /*back_adjust*/,
        const std::string& /*prepost*/,
        bool /*proxy*/,
        int /*rounding*/
    ) {
        // Validate inputs
        validate_inputs(period_days, interval);

        // Build parameters for API request
        std::map<std::string, std::string> params;
        params["period"] = Utils::days_to_period_string(period_days);
        params["interval"] = interval;
        params["events"] = "div,splits";

        // Additional parameters based on options
        if (auto_adjust) {
            params["adj"] = "true";
        }

        std::string path = "/v8/finance/chart/" + symbol_;
        return data_provider_->get_raw_data(symbol_, path, params);
    }

    nlohmann::json Ticker::get_info() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "assetProfile,summaryProfile,summaryDetail,quoteType,fundProfile,price,defaultKeyStatistics,financialData,calendarEvents";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        // Extract the info from the response
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return result[0]; // Return the first result
            }
        }
        
        return nlohmann::json(); // Return empty JSON if not found
    }

    nlohmann::json Ticker::get_recommendations() {
        std::string path = "/v6/finance/quote/news";
        std::map<std::string, std::string> params;
        params["symbol"] = symbol_;
        
        return data_provider_->get_raw_data(symbol_, path, params);
    }

    nlohmann::json Ticker::get_calendar() {
        std::string path = "/v1/finance/calendar/earnings";
        std::map<std::string, std::string> params;
        params["symbol"] = symbol_;
        
        return data_provider_->get_raw_data(symbol_, path, params);
    }

    nlohmann::json Ticker::get_earnings() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "earnings";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "earnings");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_earnings_dates() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "calendarEvents";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "calendarEvents");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_financials() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "financialData";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "financialData");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_quarterly_financials() {
        // Similar to get_financials but with different module or processing
        return get_financials(); // Simplified for now
    }

    nlohmann::json Ticker::get_major_holders() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "institutionOwnership,majorDirectHolders,majorHoldersBreakdown";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "majorHoldersBreakdown");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_institutional_holders() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "institutionOwnership";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "institutionOwnership");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_mutualfund_holders() {
        // Similar implementation to institutional holders but with different module
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "fundOwnership";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "fundOwnership");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_dividends() {
        // Dividends are typically part of the history data with events
        std::string path = "/v8/finance/chart/" + symbol_;
        std::map<std::string, std::string> params;
        params["interval"] = "1d";
        params["events"] = "div";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        // Extract dividends from the response
        if (JsonParser::has_field(response, "chart") &&
            JsonParser::has_field(JsonParser::extract_field(response, "chart"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "chart"), "result");
            if (!result.empty()) {
                if (JsonParser::has_field(result[0], "events")) {
                    return JsonParser::extract_field(result[0], "events");
                }
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_splits() {
        std::string path = "/v8/finance/chart/" + symbol_;
        std::map<std::string, std::string> params;
        params["interval"] = "1d";
        params["events"] = "splits";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        // Extract splits from the response
        if (JsonParser::has_field(response, "chart") &&
            JsonParser::has_field(JsonParser::extract_field(response, "chart"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "chart"), "result");
            if (!result.empty()) {
                if (JsonParser::has_field(result[0], "events")) {
                    return JsonParser::extract_field(result[0], "events");
                }
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_actions() {
        std::string path = "/v8/finance/chart/" + symbol_;
        std::map<std::string, std::string> params;
        params["interval"] = "1d";
        params["events"] = "div,splits";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "chart") &&
            JsonParser::has_field(JsonParser::extract_field(response, "chart"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "chart"), "result");
            if (!result.empty()) {
                if (JsonParser::has_field(result[0], "events")) {
                    return JsonParser::extract_field(result[0], "events");
                }
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_sustainability() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "esgScores";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "esgScores");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_recommendations_summary() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "recommendationTrend";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "recommendationTrend");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_company_officers() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "assetProfile";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                auto asset_profile = JsonParser::extract_field(result[0], "assetProfile");
                if (JsonParser::has_field(asset_profile, "companyOfficers")) {
                    return JsonParser::extract_field(asset_profile, "companyOfficers");
                }
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_balance_sheet() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "balanceSheetHistory";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "balanceSheetHistory");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_quarterly_balance_sheet() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "balanceSheetHistoryQuarterly";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "balanceSheetHistoryQuarterly");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_income_stmt() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "incomeStatementHistory";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "incomeStatementHistory");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_quarterly_income_stmt() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "incomeStatementHistoryQuarterly";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "incomeStatementHistoryQuarterly");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_cashflow() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "cashFlowStatementHistory";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "cashFlowStatementHistory");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_quarterly_cashflow() {
        std::string path = "/v10/finance/quoteSummary/" + symbol_;
        std::map<std::string, std::string> params;
        params["modules"] = "cashFlowStatementHistoryQuarterly";
        
        auto response = data_provider_->get_raw_data(symbol_, path, params);
        
        if (JsonParser::has_field(response, "quoteSummary") &&
            JsonParser::has_field(JsonParser::extract_field(response, "quoteSummary"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(response, "quoteSummary"), "result");
            if (!result.empty()) {
                return JsonParser::extract_field(result[0], "cashFlowStatementHistoryQuarterly");
            }
        }
        
        return nlohmann::json();
    }

    nlohmann::json Ticker::get_options() {
        std::string path = "/v7/finance/options/" + symbol_;
        
        return data_provider_->get_raw_data(symbol_, path);
    }

    nlohmann::json Ticker::get_options_for_date(const std::string& date) {
        std::string path = "/v7/finance/options/" + symbol_;
        std::map<std::string, std::string> params;
        params["date"] = date;  // date should be UNIX timestamp
        
        return data_provider_->get_raw_data(symbol_, path, params);
    }

    std::vector<std::string> Ticker::get_option_dates() {
        auto options_data = get_options();
        
        std::vector<std::string> dates;
        
        if (JsonParser::has_field(options_data, "optionChain") &&
            JsonParser::has_field(JsonParser::extract_field(options_data, "optionChain"), "result")) {
            
            auto result = JsonParser::extract_field(JsonParser::extract_field(options_data, "optionChain"), "result");
            if (!result.empty() && 
                JsonParser::has_field(result[0], "expirationDates")) {
                
                auto expiration_dates = JsonParser::extract_field(result[0], "expirationDates");
                for (const auto& date_val : expiration_dates) {
                    dates.push_back(date_val.dump());  // Convert to string representation
                }
            }
        }
        
        return dates;
    }

    nlohmann::json Ticker::get_news() {
        std::string path = "/v1/finance/search";
        std::map<std::string, std::string> params;
        params["q"] = symbol_;
        
        return data_provider_->get_raw_data(symbol_, path, params);
    }

    void Ticker::validate_inputs(int period_days, const std::string& interval) {
        if (period_days <= 0) {
            throw std::invalid_argument("Period days must be positive");
        }
        
        // Valid intervals based on Yahoo Finance API
        std::vector<std::string> valid_intervals = {
            "1m", "2m", "5m", "15m", "30m", "60m", "90m", 
            "1h", "1d", "5d", "1wk", "1mo", "3mo"
        };
        
        if (std::find(valid_intervals.begin(), valid_intervals.end(), interval) == valid_intervals.end()) {
            throw std::invalid_argument("Invalid interval: " + interval);
        }
    }

} // namespace yfinance