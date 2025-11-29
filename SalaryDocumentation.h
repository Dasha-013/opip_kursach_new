#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <ctime>
#include "PayrollSession.h"

class Employee; 
class PayrollSession;

class SalarySlip {//расчетная документация(лист)
private:
    const int codeLength = 8;
    std::string slipCode;//уникальный код ведомости
    double amount;//сумма начисления
    std::string type;//тип: "Оклад", "Премия", "Сверхурочные" и т.д.
    std::time_t issueDate;//дата выдачи
    std::shared_ptr<Employee> employee; 
    std::shared_ptr<PayrollSession> session;
    void generateCode();//генерация уникального кода

public:
    SalarySlip(double amount = 0, const std::string& type = "Оклад",
    std::shared_ptr<Employee> employee = nullptr, 
    std::time_t paymentDate = 0);

    std::shared_ptr<PayrollSession> getSession() const;
    std::shared_ptr<Employee> getEmployee() const;
    std::string getCode() const;
    double getAmount() const;
    std::string getType() const;
    std::time_t getDate() const;

    void setAmount(double newAmount);
    void setType(const std::string& newType);
    void setSession(std::shared_ptr<PayrollSession> newSession);

    std::istream& read(std::istream& in);
    std::ostream& write(std::ostream& out);
    bool operator<(const SalarySlip& other) const;
    bool operator==(const SalarySlip& other) const;
    void show() const;
    void showAsTable() const;
};