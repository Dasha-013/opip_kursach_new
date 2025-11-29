#pragma once
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include "Functions.h"
class SalaryDateTime {
    time_t date;

public:
    SalaryDateTime();
    SalaryDateTime(time_t t);
    SalaryDateTime(int day, int month, int year);
    SalaryDateTime(int day, int month, int year, int hour = 0, int minute = 0) {
        setDate(year, month, day, hour, minute);
    }
    operator time_t() const { return date; }
    void setDate(int year, int month, int day, int hour = 0, int minute = 0);
    int getYear() const;
    int getMonth() const;
    int getDay() const;
    int getHour() const;
    int getMinute() const;

    // Метод для получения последнего дня месяца
    static int getLastDayOfMonth(int year, int month);
    bool operator<(const SalaryDateTime& other) const;
    bool operator==(const SalaryDateTime& other) const;
    bool operator!=(const SalaryDateTime& other) const;

    std::string formatDate() const;

    std::istream& read(std::istream& in);
    std::ostream& write(std::ostream& out) const;

    friend std::istream& operator>>(std::istream& in, SalaryDateTime& dt);
    friend std::ostream& operator<<(std::ostream& out, const SalaryDateTime& dt);
};