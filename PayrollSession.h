#pragma once
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "SalaryDateTime.h"
#include "SalaryDocumentation.h"

class EmployeeUser;
class SalarySlip;

class PayrollSession : public std::enable_shared_from_this<PayrollSession> {
    friend class EmployeeUser;

    int sessionID;

    std::string periodLabel; 
    SalaryDateTime sessionDate;
    std::vector<std::shared_ptr<SalarySlip>> salarySlips;
    std::map<std::string, std::shared_ptr<PayrollSession>> association;
public:
    void addSlip(std::shared_ptr<SalarySlip> salarySlip) {
        salarySlips.push_back(salarySlip);
    }
    PayrollSession(int id, const std::string& period, SalaryDateTime date);
    PayrollSession(const std::string& period, SalaryDateTime date);
    PayrollSession();
    ~PayrollSession();

    friend std::ostream& operator<<(std::ostream& out, PayrollSession& session);

    void showSlips();

    int getID() const;
    std::string getPeriodLabel() const;

    SalaryDateTime getDate() const;
    std::shared_ptr<SalarySlip> getSlip(int index);

    void setID(int id);
    void setPeriodLabel(const std::string& label);
    void setDate(SalaryDateTime date);
    void setSession(std::shared_ptr<PayrollSession> s);
    void generateSlips(const std::vector<std::shared_ptr<EmployeeUser>>& employees, double baseSalary);

    std::shared_ptr<SalarySlip> chooseSlip();

    std::istream& read(std::istream& in);
    std::ostream& write(std::ostream& out);

    bool operator<(const PayrollSession& other) const;
    bool operator==(const PayrollSession& other) const;
};

struct payrollSessionCmp {
    bool operator()(const std::shared_ptr<PayrollSession>& a,
        const std::shared_ptr<PayrollSession>& b) const {
        return a->getID() < b->getID();
    }
};