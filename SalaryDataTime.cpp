#define _CRT_SECURE_NO_WARNINGS
#include "Functions.h"
#include "SalaryDateTime.h"
#include <sstream>
#include <iomanip>
#include <ctime>

SalaryDateTime::SalaryDateTime() : date(time(nullptr)) {}
SalaryDateTime::SalaryDateTime(time_t t) : date(t) {}
SalaryDateTime::SalaryDateTime(int day, int month, int year) {
    setDate(year, month, day);  // Год, месяц, день - в таком порядке!
}

void SalaryDateTime::setDate(int year, int month, int day, int hour, int minute) {
    if (year < 1900 || year > 2025) {
        throw std::invalid_argument("Недействительный ГОД");
    }
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Недействительный МЕСЯЦ");
    }
    if (day < 1 || day > 31) {
        throw std::invalid_argument("Недействительный ДЕНЬ");
    }
    if (hour < 0 || hour > 23) {
        throw std::invalid_argument("Недействительное ВРЕМЯ");
    }
    if (minute < 0 || minute > 59) {
        throw std::invalid_argument("Недействительные МИНУТЫ");
    }

    struct tm timeinfo = {};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = 0;

    date = mktime(&timeinfo);

    if (date == -1) {
        throw std::invalid_argument("Invalid date");
    }
}

std::string SalaryDateTime::formatDate() const {
    struct tm timeinfo;
    localtime_s(&timeinfo, &date);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M", &timeinfo);

    return std::string(buffer);
}

std::istream& SalaryDateTime::read(std::istream& in) {
    in.read((char*)&date, sizeof(date));
    return in;
}
std::ostream& SalaryDateTime::write(std::ostream& out) const {
    out.write((char*)&date, sizeof(date));
    return out;
}

std::istream& operator>>(std::istream& in, SalaryDateTime& dt) {
    int day, month, year;
    char separator1, separator2;

    in >> day >> separator1 >> month >> separator2 >> year;//DD.MM.YYYY
    if (separator1 != '.' || separator2 != '.') {
        in.setstate(std::ios::failbit);
        return in;
    }

    try {
        dt.setDate(year, month, day);
    }
    catch (const std::exception& e) {
        in.setstate(std::ios::failbit);
    }

    return in;
}
std::ostream& operator<<(std::ostream& out, const SalaryDateTime& dt) {
    std::tm* timeinfo = std::localtime(&dt.date);

    //DD.MM.YYYY HH:MM
    out << std::setfill('0')
        << std::setw(2) << timeinfo->tm_mday << "."
        << std::setw(2) << (timeinfo->tm_mon + 1) << "."
        << (timeinfo->tm_year + 1900);

    if (timeinfo->tm_hour != 0 || timeinfo->tm_min != 0) {
        out << " " << std::setw(2) << timeinfo->tm_hour << ":"
            << std::setw(2) << timeinfo->tm_min;
    }

    return out;
}
bool SalaryDateTime::operator<(const SalaryDateTime& other) const { return date < other.date; }
bool SalaryDateTime::operator==(const SalaryDateTime& other) const { return date == other.date; }
bool SalaryDateTime::operator!=(const SalaryDateTime& other) const { return date != other.date; }

// ДОБАВИТЬ в конец SalaryDateTime.cpp

int SalaryDateTime::getYear() const {
    std::tm* timeinfo = std::localtime(&date);
    return timeinfo->tm_year + 1900;
}

int SalaryDateTime::getMonth() const {
    std::tm* timeinfo = std::localtime(&date);
    return timeinfo->tm_mon + 1;
}

int SalaryDateTime::getDay() const {
    std::tm* timeinfo = std::localtime(&date);
    return timeinfo->tm_mday;
}

int SalaryDateTime::getHour() const {
    std::tm* timeinfo = std::localtime(&date);
    return timeinfo->tm_hour;
}

int SalaryDateTime::getMinute() const {
    std::tm* timeinfo = std::localtime(&date);
    return timeinfo->tm_min;
}

int SalaryDateTime::getLastDayOfMonth(int year, int month) {
    if (month < 1 || month > 12) return 30;

    // Месяцы с 31 днем
    if (month == 1 || month == 3 || month == 5 || month == 7 ||
        month == 8 || month == 10 || month == 12) {
        return 31;
    }
    // Февраль
    else if (month == 2) {
        // Високосный год
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 29;
        }
        else {
            return 28;
        }
    }
    // Остальные месяцы
    else {
        return 30;
    }
}