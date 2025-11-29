#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include "SalaryDocumentation.h"

namespace TableFormatter {

    class Formatter {
    private:
        std::vector<std::string> headers;
        std::vector<std::vector<std::string>> rows;
        std::vector<int> columnWidths;

    public:
        Formatter(const std::vector<std::string>& headers);

        void addRow(const std::vector<std::string>& row);
        void display() const;
        void displayHorizontalLine() const;
        static std::string centerString(const std::string& str, int width);
    };

    void displaySalarySlipTable(const std::vector<std::shared_ptr<class SalarySlip>>& slips);
    void displayDetailedSalarySlip(std::shared_ptr<class SalarySlip> slip);

    std::string formatDate(std::time_t time);
    std::string formatCurrency(double amount);
}