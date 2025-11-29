#include "TableFormatter.h"
#include "Employee.h"
#include "SalaryDocumentation.h"
#include <iomanip>
#include <ctime>

namespace TableFormatter {

    Formatter::Formatter(const std::vector<std::string>& headers)
        : headers(headers) {
        for (const auto& header : headers) {
            columnWidths.push_back(header.length() + 4);
        }
    }

    void Formatter::addRow(const std::vector<std::string>& row) {
        if (row.size() != headers.size()) {
            throw std::invalid_argument("Количество элементов в строке не соответствует заголовкам");
        }

        rows.push_back(row);

        for (size_t i = 0; i < row.size(); ++i) {
            if (row[i].length() + 4 > columnWidths[i]) {
                columnWidths[i] = row[i].length() + 4;
            }
        }
    }

    void Formatter::displayHorizontalLine() const {
        int totalWidth = 0;
        for (int width : columnWidths) {
            totalWidth += width + 1;
        }
        totalWidth += 1;

        std::cout << "╔";
        for (size_t i = 0; i < columnWidths.size(); ++i) {
            for (int j = 0; j < columnWidths[i]; ++j) {
                std::cout << "═";
            }
            if (i < columnWidths.size() - 1) {
                std::cout << "╦";
            }
        }
        std::cout << "╗\n";
    }

    std::string Formatter::centerString(const std::string& str, int width) {
        if (str.length() >= width) {
            return str;
        }

        int padding = width - str.length();
        int leftPadding = padding / 2;
        int rightPadding = padding - leftPadding;

        return std::string(leftPadding, ' ') + str + std::string(rightPadding, ' ');
    }

    void Formatter::display() const {
        displayHorizontalLine();

        std::cout << "║";
        for (size_t i = 0; i < headers.size(); ++i) {
            std::cout << centerString(headers[i], columnWidths[i]) << "║";
        }
        std::cout << "\n";

        std::cout << "╠";
        for (size_t i = 0; i < columnWidths.size(); ++i) {
            for (int j = 0; j < columnWidths[i]; ++j) {
                std::cout << "═";
            }
            if (i < columnWidths.size() - 1) {
                std::cout << "╬";
            }
        }
        std::cout << "╣\n";

        for (const auto& row : rows) {
            std::cout << "║";
            for (size_t i = 0; i < row.size(); ++i) {
                std::cout << centerString(row[i], columnWidths[i]) << "║";
            }
            std::cout << "\n";
        }

        std::cout << "╚";
        for (size_t i = 0; i < columnWidths.size(); ++i) {
            for (int j = 0; j < columnWidths[i]; ++j) {
                std::cout << "═";
            }
            if (i < columnWidths.size() - 1) {
                std::cout << "╩";
            }
        }
        std::cout << "╝\n";
    }

    // Вспомогательные функции
    std::string formatDate(std::time_t time) {
        std::tm timeinfo;
        localtime_s(&timeinfo, &time);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", &timeinfo);
        return std::string(buffer);
    }

    std::string formatCurrency(double amount) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << amount << " руб.";
        return ss.str();
    }

    // Функции для ведомостей
    void displaySalarySlipTable(const std::vector<std::shared_ptr<SalarySlip>>& slips) {
        if (slips.empty()) {
            std::cout << "╔════════════════════════════════════════╗\n";
            std::cout << "║         ВЕДОМОСТИ НЕ НАЙДЕНЫ          ║\n";
            std::cout << "╚════════════════════════════════════════╝\n";
            return;
        }

        std::vector<std::string> headers = {
            "Код ведомости", "Тип выплаты", "Сумма", "Дата выплаты", "Сотрудник"
        };

        Formatter table(headers);

        for (const auto& slip : slips) {
            std::vector<std::string> row;
            row.push_back(slip->getCode());
            row.push_back(slip->getType());
            row.push_back(formatCurrency(slip->getAmount()));
            row.push_back(formatDate(slip->getDate()));

            auto employee = slip->getEmployee();
            if (employee) {
                row.push_back(employee->getName());
            }
            else {
                row.push_back("Не указан");
            }

            table.addRow(row);
        }

        std::cout << "\n╔══════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                     ТАБЛИЦА ВЕДОМОСТЕЙ ЗАРПЛАТ                     ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";

        table.display();

        std::cout << "Всего ведомостей: " << slips.size() << "\n";
    }

    void displayDetailedSalarySlip(std::shared_ptr<SalarySlip> slip) {
        if (!slip) {
            std::cout << "╔════════════════════════════════════════╗\n";
            std::cout << "║         ВЕДОМОСТЬ НЕ НАЙДЕНА          ║\n";
            std::cout << "╚════════════════════════════════════════╝\n";
            return;
        }

        auto employee = slip->getEmployee();
        auto session = slip->getSession();

        std::cout << "\n╔══════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   ДЕТАЛЬНАЯ ВЕДОМОСТЬ ЗАРПЛАТЫ                    ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";

        std::vector<std::string> headers = { "Параметр", "Значение" };
        Formatter table(headers);

        table.addRow({ "Код ведомости", slip->getCode() });
        table.addRow({ "Тип выплаты", slip->getType() });
        table.addRow({ "Сумма выплаты", formatCurrency(slip->getAmount()) });
        table.addRow({ "Дата выплаты", formatDate(slip->getDate()) });

        if (employee) {
            table.addRow({ "Сотрудник", employee->getName() });
            table.addRow({ "Должность", employee->getPosition() });
            table.addRow({ "Расчетная зарплата", formatCurrency(employee->calculateSalary()) });

            // Показываем разницу для понимания
            if (slip->getType() == "Аванс") {
                double remaining = employee->calculateSalary() - slip->getAmount();
                table.addRow({ "Остаток к выплате", formatCurrency(remaining) });
            }
            else if (slip->getType() == "Зарплата") {
                table.addRow({ "Статус", "Полная выплата за период" });
            }
        }
        else {
            table.addRow({ "Сотрудник", "Не указан" });
        }

        if (session) {
            table.addRow({ "Период расчета", session->getPeriodLabel() });
        }

        table.display();
    }
}