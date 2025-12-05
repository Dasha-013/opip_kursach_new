#pragma once
#include "PayrollSession.h"
#include <set>
#include <memory>
#include <string>

class EmployeeUser;

class PayrollPeriod : public std::enable_shared_from_this<PayrollPeriod> {
    friend class EmployeeUser;

    std::string label;
    std::string description; 
    SalaryDateTime duration; 
    std::set<std::shared_ptr<PayrollSession>, payrollSessionCmp> sessions;

public:
    PayrollPeriod();
    ~PayrollPeriod();

    void addSession(std::shared_ptr<PayrollSession> session);

    friend std::ostream& operator<<(std::ostream& out, PayrollPeriod& period);

    bool operator!=(PayrollPeriod& other);
    bool operator==(PayrollPeriod& other);
    bool operator<(PayrollPeriod& other);

    void getInfo();

    std::ostream& write(std::ostream& out);
    std::istream& read(std::istream& in);

    void setLabel(const std::string& label);
    void setDescription(const std::string& description);
    void setDuration(const SalaryDateTime& dt);

    std::string getLabel() const;
    std::string getDescription() const;
    SalaryDateTime getDuration() const;

    std::shared_ptr<PayrollSession> chooseSession();
    void showSessions() const;
    size_t getSessionsCount() const { return sessions.size(); }
};

struct payrollPeriodCmp {
    bool operator()(const std::shared_ptr<PayrollPeriod>& p1, const std::shared_ptr<PayrollPeriod>& p2) const;//сортировка периодов по дате 
};
