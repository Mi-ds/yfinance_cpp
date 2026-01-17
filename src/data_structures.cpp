#include "data_structures.h"
#include <iostream>

namespace yfinance {

    void DataFrame::print() const {
        // Print column headers
        for (const auto& col_name : column_names_) {
            std::cout << col_name << "\t";
        }
        std::cout << std::endl;
        
        // Print each row
        for (size_t row_idx = 0; row_idx < num_rows_; ++row_idx) {
            for (const auto& col : columns_) {
                DataValue val = (*col)[row_idx];
                
                // Print the value based on its type
                std::visit([](const auto& value) {
                    std::cout << value << "\t";
                }, val);
            }
            std::cout << std::endl;
        }
    }

} // namespace yfinance