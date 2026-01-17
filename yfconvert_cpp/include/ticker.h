#ifndef TICKER_H
#define TICKER_H

#include <string>
#include <memory>
#include <vector>
#include <map>

#include "yf_data.h"
#include "json_parser.h"

namespace yfinance {

    /**
     * @brief Represents a single stock ticker with all its data
     */
    class Ticker {
    public:
        explicit Ticker(const std::string& symbol);
        ~Ticker();

        // Get the ticker symbol
        std::string get_symbol() const;

        // Fetch historical price data
        nlohmann::json history(
            int period_days = 365,  // Default to 1 year of data
            const std::string& interval = "1d",
            bool auto_adjust = true,
            bool back_adjust = false,
            const std::string& prepost = "false",
            bool proxy = false,
            int rounding = 0
        );

        // Get company information
        nlohmann::json get_info();

        // Get recommendations
        nlohmann::json get_recommendations();

        // Get calendar events (earnings, etc.)
        nlohmann::json get_calendar();

        // Get earnings data
        nlohmann::json get_earnings();

        // Get quarterly earnings
        nlohmann::json get_earnings_dates();

        // Get financials
        nlohmann::json get_financials();

        // Get quarterly financials
        nlohmann::json get_quarterly_financials();

        // Get major holders
        nlohmann::json get_major_holders();

        // Get institutional holders
        nlohmann::json get_institutional_holders();

        // Get mutual fund holders
        nlohmann::json get_mutualfund_holders();

        // Get dividends
        nlohmann::json get_dividends();

        // Get splits
        nlohmann::json get_splits();

        // Get actions (dividends + splits)
        nlohmann::json get_actions();

        // Get sustainability info
        nlohmann::json get_sustainability();

        // Get analysts recommendations
        nlohmann::json get_recommendations_summary();

        // Get company officers
        nlohmann::json get_company_officers();

        // Get balance sheet
        nlohmann::json get_balance_sheet();

        // Get quarterly balance sheet
        nlohmann::json get_quarterly_balance_sheet();

        // Get income statement
        nlohmann::json get_income_stmt();

        // Get quarterly income statement
        nlohmann::json get_quarterly_income_stmt();

        // Get cash flow statement
        nlohmann::json get_cashflow();

        // Get quarterly cash flow statement
        nlohmann::json get_quarterly_cashflow();

        // Get options data
        nlohmann::json get_options();

        // Get options for specific date
        nlohmann::json get_options_for_date(const std::string& date);

        // Get all option dates
        std::vector<std::string> get_option_dates();

        // Get news
        nlohmann::json get_news();

    private:
        std::string symbol_;
        std::unique_ptr<YfData> data_provider_;

        // Helper method to validate inputs
        void validate_inputs(int period_days, const std::string& interval);
    };

} // namespace yfinance

#endif // TICKER_H