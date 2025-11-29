#pragma once

#include <string>
#include <iostream>
#include "Functions.h"  

class Employee {
public:
    virtual ~Employee() = default;

    virtual std::string getName() const = 0;
    virtual std::string getPosition() const = 0;
    virtual double calculateSalary() const = 0;
    virtual void showDetailedInfo() const = 0;
    virtual void edit() = 0;
    virtual void read(std::istream& in) = 0;
    virtual void write(std::ostream& out) const = 0;
    virtual std::string getID() const = 0; 
     
};

  class ConcreteEmployee : public Employee {
  protected:
    std::string name;
    std::string position;
    std::string id;
    double baseSalary;

public:
    ConcreteEmployee() = default;

    ConcreteEmployee(const std::string& name, const std::string& position, double salary)
        : name(name), position(position), baseSalary(salary) {
        id = name + "_" + std::to_string(std::time(nullptr));}
    std::string getID() const override { return id; }
    std::string getName() const override { return name; }
    std::string getPosition() const override { return position; }
    double calculateSalary() const override { return baseSalary; }

    void edit() override {
        get("Введите ФИО: ", name);
        get("Введите должность: ", position);
        get("Введите базовую зарплату: ", baseSalary);
    }

    void read(std::istream& in) override {
        bReadStr(name, in);
        bReadStr(position, in);
        in.read(reinterpret_cast<char*>(&baseSalary), sizeof(baseSalary));
    }

    void write(std::ostream& out) const override {
        bWriteStr(name, out);
        bWriteStr(position, out);
        out.write(reinterpret_cast<const char*>(&baseSalary), sizeof(baseSalary));
    }
    void showDetailedInfo() const override {
        std::cout << "=== ИНФОРМАЦИЯ О СОТРУДНИКЕ ===\n";
        std::cout << "ФИО: " << name << "\n";
        std::cout << "Должность: " << position << "\n";
        std::cout << "Базовая зарплата: " << baseSalary << "\n";
        std::cout << "Расчетная зарплата: " << calculateSalary() << "\n";
    }
    void setBaseSalary(double salary) { baseSalary = salary; }
    double getBaseSalary() const { return baseSalary; }

    friend std::ostream& operator<<(std::ostream& os, const ConcreteEmployee& emp) {//перегрузка оператора вывода 
        os << "Сотрудник: " << emp.name
            << " | Должность: " << emp.position
            << " | Зарплата: " << emp.baseSalary;
        return os;
    }
};

class RegularEmployee : public ConcreteEmployee {
public:
    RegularEmployee() = default;
    RegularEmployee(const std::string& name, const std::string& position, double salary)
        : ConcreteEmployee(name, position, salary) {}

    double calculateSalary() const override {
        return getBaseSalary(); // Просто базовая зарплата
    }

    void showDetailedInfo() const override {
        std::cout << "=== ОБЫЧНЫЙ СОТРУДНИК ===\n";
        ConcreteEmployee::showDetailedInfo();
        std::cout << "ИТОГО: " << calculateSalary() << "\n";
    }
};
class ManagerEmployee : public ConcreteEmployee {
    double bonusPercentage;

public:
    ManagerEmployee() : bonusPercentage(0.2) {} 
    ManagerEmployee(const std::string& name, double salary, double bonus = 0.2)
        : ConcreteEmployee(name, "Менеджер", salary), bonusPercentage(bonus) {}

    double calculateSalary() const override {
        return getBaseSalary() + (getBaseSalary() * bonusPercentage);
    }

    void edit() override {
        ConcreteEmployee::edit();
        get("Введите бонусный процент (0.1 = 10%): ", bonusPercentage);
    }

    void read(std::istream& in) override {
        ConcreteEmployee::read(in);
        in.read(reinterpret_cast<char*>(&bonusPercentage), sizeof(bonusPercentage));
    }

    void write(std::ostream& out) const override {
        ConcreteEmployee::write(out);
        out.write(reinterpret_cast<const char*>(&bonusPercentage), sizeof(bonusPercentage));
    }

    void showDetailedInfo() const override {
        std::cout << "=== МЕНЕДЖЕР ===\n";
        ConcreteEmployee::showDetailedInfo();
        std::cout << "Бонус (" << (bonusPercentage * 100) << "%): "
            << (getBaseSalary() * bonusPercentage) << "\n";
        std::cout << "ИТОГО: " << calculateSalary() << "\n";
    }
};
class SalesEmployee : public ConcreteEmployee {
    double salesAmount;
    double commissionRate;

public:
    SalesEmployee() : salesAmount(0), commissionRate(0.1) {}
    SalesEmployee(const std::string& name, double salary, double sales = 0, double commission = 0.1)
        : ConcreteEmployee(name, "Продавец", salary), salesAmount(sales), commissionRate(commission) {}

    double calculateSalary() const override {
        return getBaseSalary() + (salesAmount * commissionRate);
    }

    void edit() override {
        ConcreteEmployee::edit();
        get("Введите сумму продаж: ", salesAmount);
        get("Введите процент комиссии (0.1 = 10%): ", commissionRate);
    }

    void read(std::istream& in) override {
        ConcreteEmployee::read(in);
        in.read(reinterpret_cast<char*>(&salesAmount), sizeof(salesAmount));
        in.read(reinterpret_cast<char*>(&commissionRate), sizeof(commissionRate));
    }

    void write(std::ostream& out) const override {
        ConcreteEmployee::write(out);
        out.write(reinterpret_cast<const char*>(&salesAmount), sizeof(salesAmount));
        out.write(reinterpret_cast<const char*>(&commissionRate), sizeof(commissionRate));
    }

    void showDetailedInfo() const override {
        std::cout << "=== ПРОДАВЕЦ ===\n";
        ConcreteEmployee::showDetailedInfo();
        std::cout << "Продажи: " << salesAmount << "\n";
        std::cout << "Комиссия (" << (commissionRate * 100) << "%): "
            << (salesAmount * commissionRate) << "\n";
        std::cout << "ИТОГО: " << calculateSalary() << "\n";
    }
};
class HourlyEmployee : public ConcreteEmployee {
    double hoursWorked;
    double hourlyRate;

public:
    HourlyEmployee() : hoursWorked(0), hourlyRate(0) {}
    HourlyEmployee(const std::string& name, double hours, double rate)
        : ConcreteEmployee(name, "Почасовой сотрудник", 0), hoursWorked(hours), hourlyRate(rate) {}

    double calculateSalary() const override {
        return hoursWorked * hourlyRate;
    }

    void edit() override {
        ConcreteEmployee::edit();
        get("Введите отработанные часы: ", hoursWorked);
        get("Введите почасовую ставку: ", hourlyRate);
        setBaseSalary(0); // Для почасовых базовая зарплата не используется
    }

    void read(std::istream& in) override {
        ConcreteEmployee::read(in);
        in.read(reinterpret_cast<char*>(&hoursWorked), sizeof(hoursWorked));
        in.read(reinterpret_cast<char*>(&hourlyRate), sizeof(hourlyRate));
    }

    void write(std::ostream& out) const override {
        ConcreteEmployee::write(out);
        out.write(reinterpret_cast<const char*>(&hoursWorked), sizeof(hoursWorked));
        out.write(reinterpret_cast<const char*>(&hourlyRate), sizeof(hourlyRate));
    }

    void showDetailedInfo() const override {
        std::cout << "=== ПОЧАСОВОЙ СОТРУДНИК ===\n";
        std::cout << "ФИО: " << name << "\n";
        std::cout << "Должность: " << position << "\n";
        std::cout << "Отработано часов: " << hoursWorked << "\n";
        std::cout << "Почасовая ставка: " << hourlyRate << "\n";
        std::cout << "ИТОГО: " << calculateSalary() << "\n";
    }
};

class BonusEmployee : public ConcreteEmployee {
    double fixedBonus;

public:
    BonusEmployee() : fixedBonus(0) {}
    BonusEmployee(const std::string& name, const std::string& position, double salary, double bonus = 0)
        : ConcreteEmployee(name, position, salary), fixedBonus(bonus) {}

    double calculateSalary() const override {
        return getBaseSalary() + fixedBonus;
    }

    void edit() override {
        ConcreteEmployee::edit();
        get("Введите фиксированную премию: ", fixedBonus);
    }

    void read(std::istream& in) override {
        ConcreteEmployee::read(in);
        in.read(reinterpret_cast<char*>(&fixedBonus), sizeof(fixedBonus));
    }

    void write(std::ostream& out) const override {
        ConcreteEmployee::write(out);
        out.write(reinterpret_cast<const char*>(&fixedBonus), sizeof(fixedBonus));
    }

    void showDetailedInfo() const override {
        std::cout << "=== СОТРУДНИК С ПРЕМИЕЙ ===\n";
        ConcreteEmployee::showDetailedInfo();
        std::cout << "Фиксированная премия: " << fixedBonus << "\n";
        std::cout << "ИТОГО: " << calculateSalary() << "\n";
    }
};