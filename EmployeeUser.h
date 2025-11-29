#pragma once
#include "PayrollPeriod.h"
#include "PayrollSession.h"
#include "SalaryDocumentation.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <set>

class EmployeeUser {
    std::string role;             
    std::string login;
    std::string surname, name;
    std::vector<std::shared_ptr<SalarySlip>> slips;
    std::map<std::string, std::shared_ptr<PayrollSession>> association;
    std::string path;

public:
    ~EmployeeUser();
    EmployeeUser(const std::string& path, const std::string& login, const std::string& role = "viewer");
    EmployeeUser(const std::string& path);

    bool isAdmin() const;
    bool isManager() const;
    bool isViewer() const;

    void loadInfo();
    void writeInfo();

    void getInfo();
    void showInfo();

    void associateSlips(std::set<std::shared_ptr<PayrollPeriod>, payrollPeriodCmp>& periods);
    void slipBack();
    void pushSlip(std::shared_ptr<SalarySlip> slip);
    void showSlips();

    std::string getLogin() const;
    std::string getSurname() const;
    std::string getName() const;
    std::string getRole() const;

    std::shared_ptr<Employee> getEmployee() const;
    void setEmployee(std::shared_ptr<Employee> emp);

private:
    std::shared_ptr<Employee> employeeProfile;
};
