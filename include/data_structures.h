#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <variant>
#include <optional>
#include <stdexcept>

namespace yfinance {

    // Variant type to hold different data types (similar to pandas flexibility)
    using DataValue = std::variant<int, double, std::string, bool>;
    
    // Column data structure to mimic pandas DataFrame columns
    struct DataColumn {
        std::string name;
        std::vector<DataValue> values;
        
        DataColumn(const std::string& col_name) : name(col_name) {}
        
        void add_value(const DataValue& val) {
            values.push_back(val);
        }
        
        size_t size() const {
            return values.size();
        }
        
        DataValue operator[](size_t index) const {
            if (index >= values.size()) {
                throw std::out_of_range("Index out of range for DataColumn");
            }
            return values[index];
        }
    };
    
    // DataFrame structure to mimic pandas DataFrame
    class DataFrame {
    private:
        std::vector<std::string> column_names_;
        std::vector<std::unique_ptr<DataColumn>> columns_;
        size_t num_rows_;
        
    public:
        DataFrame() : num_rows_(0) {}
        
        // Add a column to the DataFrame
        void add_column(const std::string& name) {
            column_names_.push_back(name);
            columns_.push_back(std::make_unique<DataColumn>(name));
        }
        
        // Get a column by name
        DataColumn* get_column(const std::string& name) {
            for (auto& col : columns_) {
                if (col->name == name) {
                    return col.get();
                }
            }
            return nullptr; // Column not found
        }
        
        // Add a row of data
        template<typename... Args>
        void add_row(Args&&... values) {
            std::vector<DataValue> row_values = {values...};
            
            if (row_values.size() != columns_.size()) {
                throw std::invalid_argument("Number of values does not match number of columns");
            }
            
            for (size_t i = 0; i < row_values.size(); ++i) {
                columns_[i]->add_value(row_values[i]);
            }
            
            num_rows_++;
        }
        
        // Get number of rows
        size_t rows() const {
            return num_rows_;
        }
        
        // Get number of columns
        size_t cols() const {
            return column_names_.size();
        }
        
        // Get column names
        const std::vector<std::string>& get_column_names() const {
            return column_names_;
        }
        
        // Get a row as a map of column name to value
        std::map<std::string, DataValue> get_row(size_t index) const {
            if (index >= num_rows_) {
                throw std::out_of_range("Row index out of range");
            }
            
            std::map<std::string, DataValue> row;
            for (const auto& col : columns_) {
                row[col->name] = (*col)[index];
            }
            return row;
        }
        
        // Print the DataFrame (for debugging)
        void print() const;
    };
    
    // Specific data structure for holding historical stock prices
    struct PriceHistory {
        std::vector<double> open;
        std::vector<double> high;
        std::vector<double> low;
        std::vector<double> close;
        std::vector<double> volume;
        std::vector<std::string> date;  // ISO 8601 date strings
        
        // Add a price entry
        void add_entry(double o, double h, double l, double c, double vol, const std::string& d) {
            open.push_back(o);
            high.push_back(h);
            low.push_back(l);
            close.push_back(c);
            volume.push_back(vol);
            date.push_back(d);
        }
        
        // Get number of entries
        size_t size() const {
            return open.size();
        }
    };

} // namespace yfinance

#endif // DATA_STRUCTURES_H