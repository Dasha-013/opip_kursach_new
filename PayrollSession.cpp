#include <sstream>
#include <iomanip>
#include <ctime>
#include "Base.h"
#include "PayrollSession.h"
#include "SalaryDocumentation.h"
#include "SalaryDateTime.h"
#include "Functions.h"
#include "EmployeeUser.h"


PayrollSession::PayrollSession() : sessionID(0) {} // или любое значение по умолчанию

PayrollSession::PayrollSession(const std::string& period, SalaryDateTime date)
    : sessionID(0), periodLabel(period), sessionDate(date) {}

PayrollSession::PayrollSession(int id, const std::string& period, SalaryDateTime date)
    : sessionID(id), periodLabel(period), sessionDate(date) {}

int PayrollSession::getID() const {
    return sessionID;
}

std::string PayrollSession::getPeriodLabel() const {
    return periodLabel;//метка периода
}

SalaryDateTime PayrollSession::getDate() const {
    return sessionDate;
}

std::shared_ptr<SalarySlip> PayrollSession::getSlip(int index) {
    if (index < 0 || index >= salarySlips.size()) {
        throw std::out_of_range("НЕВЕРНЫЙ ИДНЕКС ВЕДОМОСТИ");
    }
    return salarySlips[index];
}

void PayrollSession::setID(int id) {
    if (id < 0) {
        throw std::invalid_argument("ID не может быть отрицательным");
    }
    sessionID = id;
}

void PayrollSession::setPeriodLabel(const std::string& label) {
    if (label.empty()) {
        throw std::invalid_argument("Период не может быть пустым");
    }
    periodLabel = label;
}

void PayrollSession::setDate(SalaryDateTime date) {
    sessionDate = date;
}

void PayrollSession::setSession(std::shared_ptr<PayrollSession> s) {
    if (!s) {
        throw std::invalid_argument("Сессия не может быть нулевой");
    }

    std::string key = std::to_string(s->getID()) + "_" + s->getPeriodLabel();
    association[key] = s;
}

// В PayrollSession.cpp - ИЗМЕНИТЬ существующий метод:
void PayrollSession::generateSlips(const std::vector<std::shared_ptr<EmployeeUser>>& employees, double baseSalary = 0) {
    salarySlips.clear();
    for (const auto& user : employees) {
        auto emp = user->getEmployee();
        if (!emp) continue;

        // Если baseSalary = 0, используем реальную зарплату, иначе - указанную
        double salary = (baseSalary > 0) ? baseSalary : emp->calculateSalary();
        auto slip = std::make_shared<SalarySlip>(salary, "Оклад", emp);
        slip->setSession(shared_from_this());
        salarySlips.push_back(slip);
        user->pushSlip(slip);
    }
}
void PayrollSession::showSlips() {
    for (size_t i = 0; i < salarySlips.size(); ++i) {
        std::cout << "[" << i + 1 << "] ";
        salarySlips[i]->show();
        std::cout << std::endl;
    }
}

std::shared_ptr<SalarySlip> PayrollSession::chooseSlip() {
    showSlips();
    int ch;
    getInRange("Выберите ведомость (0 - отмена): ", ch, 0, salarySlips.size());
    return ch == 0 ? nullptr : salarySlips[ch - 1];
}

std::istream& PayrollSession::read(std::istream& in) {
    in.read((char*)&sessionID, sizeof(sessionID));
    bReadStr(periodLabel, in);
    sessionDate.read(in);
    int count;
    in.read((char*)&count, sizeof(count));
    salarySlips.clear();
    for (int i = 0; i < count; ++i) {
        auto slip = std::make_shared<SalarySlip>();
        slip->read(in);
        salarySlips.push_back(slip);
    }
    return in;
}

std::ostream& PayrollSession::write(std::ostream& out) {
    out.write((char*)&sessionID, sizeof(sessionID));
    bWriteStr(periodLabel, out);
    sessionDate.write(out);
    int count = salarySlips.size();
    out.write((char*)&count, sizeof(count));
    for (const auto& slip : salarySlips)
        slip->write(out);
    return out;
}

std::ostream& operator<<(std::ostream& out, PayrollSession& s) {
    out << "Сессия #" << s.getID() << " | Период: " << s.getPeriodLabel()
        << " | Дата: " << s.getDate().formatDate();
    return out;
}

bool PayrollSession::operator<(const PayrollSession& s) const { return sessionDate < s.sessionDate; }
bool PayrollSession::operator==(const PayrollSession& s) const { return sessionID == s.sessionID; }

PayrollSession::~PayrollSession() {}