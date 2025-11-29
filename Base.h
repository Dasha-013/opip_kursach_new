#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <fstream>
#include <algorithm>
#include "PayrollPeriod.h"
#include "Employee.h"

// Шаблонный класс Repository
template<typename T, typename Comparator = std::less<T>>
class Repository {
private:
    std::set<std::shared_ptr<T>, Comparator> items;
    std::string filename;

public:
    Repository(const std::string& file = "") : filename(file) {}

    void add(std::shared_ptr<T> item) {
        items.insert(item);
    }

    bool remove(std::shared_ptr<T> item) {
        return items.erase(item) > 0;
    }

    std::shared_ptr<T> find(const std::string& id) const {
        auto it = std::find_if(items.begin(), items.end(),
            [&](const auto& item) { return item->getID() == id; });
        return it != items.end() ? *it : nullptr;
    }

    template<typename Predicate>
    std::vector<std::shared_ptr<T>> filter(Predicate pred) const {
        std::vector<std::shared_ptr<T>> result;
        std::copy_if(items.begin(), items.end(), std::back_inserter(result), pred);
        return result;
    }

    template<typename Function>
    void forEach(Function func) const {
        for (const auto& item : items) {
            func(item);
        }
    }

    std::vector<std::shared_ptr<T>> getAll() const {
        return std::vector<std::shared_ptr<T>>(items.begin(), items.end());
    }

    size_t size() const { return items.size(); }
    bool empty() const { return items.empty(); }

    void clear() { items.clear(); }
    auto begin() const { return items.begin(); }
    auto end() const { return items.end(); }
};

// Компаратор для сотрудников
struct payrollEmployeeCmp {
    bool operator()(const std::shared_ptr<Employee>& a,
        const std::shared_ptr<Employee>& b) const {
        if (!a || !b) return false;
        return a->getName() < b->getName();
    }
};

class Base {
public:
    virtual ~Base() = default;
    virtual void run() = 0;
};

class EmployeeBase : public Base {
protected:
    Repository<Employee, payrollEmployeeCmp> employeesRepo;

    virtual void showHeader() = 0;
    virtual void readEmployees();

public:
    EmployeeBase();
    virtual ~EmployeeBase();

    void showAll();
    std::shared_ptr<Employee> chooseEmployee();
    void createTestEmployees();
    virtual void employeeMenu(std::shared_ptr<Employee> emp) = 0;
    virtual void writeEmployees();

    // Шаблонные методы
    template<typename Predicate>
    std::vector<std::shared_ptr<Employee>> findEmployees(Predicate pred) const {
        return employeesRepo.filter(pred);
    }

    template<typename Function>
    void forEachEmployee(Function func) const {
        employeesRepo.forEach(func);
    }

    template<typename Calculator>
    double calculateTotalSalary(Calculator calc) const {
        double total = 0.0;
        employeesRepo.forEach([&](auto emp) {
            total += calc(emp);
            });
        return total;
    }

    // Геттер для доступа к репозиторию (для совместимости)
    const Repository<Employee, payrollEmployeeCmp>& getEmployeesRepo() const {
        return employeesRepo;
    }
};

class AdminEmployeeBase : public EmployeeBase {
private:
    static int adminInstanceCount;

protected:
    std::shared_ptr<EmployeeUser> adminUser;
    std::vector<std::shared_ptr<EmployeeUser>> employeeUsers;
    Repository<PayrollPeriod, payrollPeriodCmp> periodsRepo;
    std::stack<std::shared_ptr<PayrollPeriod>> deletedPeriodsStack;
    std::queue<std::shared_ptr<PayrollPeriod>> deletionQueue;

    void showHead();
    void showPeriods();
    void addPeriod();
    std::shared_ptr<PayrollPeriod> choosePeriod();
    void deletePeriodWithStack();
    void undoDelete();
    void processDeletionQueue();
    void showDeletedPeriods();
    void showSystemInfo();
    void periodMenu(std::shared_ptr<PayrollPeriod> period);
    void sessionMenu(std::shared_ptr<PayrollSession> session);
    void slipMenu(std::shared_ptr<SalarySlip> slip);

public:
    AdminEmployeeBase(std::shared_ptr<EmployeeUser> u);
    ~AdminEmployeeBase();
    void showHeader() override;
    void run() override;
    void employeeMenu(std::shared_ptr<Employee> emp) override;
    static int getActiveAdminCount();

    // Шаблонные методы для периодов
    template<typename Predicate>
    std::vector<std::shared_ptr<PayrollPeriod>> findPeriods(Predicate pred) const {
        return periodsRepo.filter(pred);
    }

    template<typename Function>
    void forEachPeriod(Function func) const {
        periodsRepo.forEach(func);
    }
};

class GuestEmployeeBase : public EmployeeBase {
public:
    GuestEmployeeBase();
    void showHeader() override;
    void run() override;
    void employeeMenu(std::shared_ptr<Employee> emp) override;
};

class UserPayrollBase : public GuestEmployeeBase {
    std::shared_ptr<EmployeeUser> user;
    std::vector<std::shared_ptr<PayrollPeriod>> userPeriods;

    void showHead();
    void showPeriods();
    std::shared_ptr<PayrollPeriod> choosePeriod();
    void periodMenu(std::shared_ptr<PayrollPeriod> period);
    void sessionMenu(std::shared_ptr<PayrollSession> session);
    void slipMenu(std::shared_ptr<SalarySlip> slip);
    void loadUserPeriods();

public:
    UserPayrollBase(std::shared_ptr<EmployeeUser> user);
    void showHeader() override;
    void run() override;
    void employeeMenu(std::shared_ptr<Employee> emp) override;

    // Шаблонные методы для пользовательских периодов
    template<typename Predicate>
    std::vector<std::shared_ptr<PayrollPeriod>> findUserPeriods(Predicate pred) const {
        std::vector<std::shared_ptr<PayrollPeriod>> result;
        std::copy_if(userPeriods.begin(), userPeriods.end(),
            std::back_inserter(result), pred);
        return result;
    }

    template<typename Function>
    void forEachUserPeriod(Function func) const {
        for (const auto& period : userPeriods) {
            func(period);
        }
    }
};