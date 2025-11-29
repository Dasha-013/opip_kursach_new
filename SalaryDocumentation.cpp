#include "SalaryDocumentation.h"
#include "PayrollSession.h"
#include "Employee.h"
#include "TableFormatter.h"
#include <iomanip>

SalarySlip::SalarySlip(double amt, const std::string& tp, std::shared_ptr<Employee> emp, std::time_t paymentDate)
    : amount(amt), type(tp), employee(emp), session(nullptr) {
    generateCode();
    // Используем переданную дату выплаты, или текущую если 0
    issueDate = (paymentDate == 0) ? std::time(nullptr) : paymentDate;
}
std::string SalarySlip::getCode() const { return slipCode; }
double SalarySlip::getAmount() const { return amount; }
std::string SalarySlip::getType() const { return type; }
std::time_t SalarySlip::getDate() const { return issueDate; }
std::shared_ptr<Employee> SalarySlip::getEmployee() const { return employee; }

void SalarySlip::setAmount(double newAmount) { amount = newAmount; }
void SalarySlip::setType(const std::string& newType) { type = newType; }

void SalarySlip::showAsTable() const {
    std::vector<std::string> headers = { "Параметр", "Значение" };
    TableFormatter::Formatter table(headers); // ← ИСПРАВЛЕНО

    table.addRow({ "Код ведомости", slipCode });
    table.addRow({ "Тип выплаты", type });

    std::stringstream amountStr;
    amountStr << std::fixed << std::setprecision(2) << amount << " руб.";
    table.addRow({ "Сумма", amountStr.str() });

    std::tm timeStruct;
    localtime_s(&timeStruct, &issueDate);
    char dateBuffer[80];
    std::strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y %H:%M", &timeStruct);
    table.addRow({ "Дата выплаты", std::string(dateBuffer) });

    std::string employeeName = employee ? employee->getName() : "Не указан";
    table.addRow({ "Сотрудник", employeeName });

    table.display();
}
void SalarySlip::generateCode() {
    slipCode.clear();
    for (int i = 0; i < codeLength; ++i)
        slipCode += (rand() % 2 ? 'A' + rand() % 26 : '0' + rand() % 10);
}

void SalarySlip::show() const {
    std::tm timeStruct;
    localtime_s(&timeStruct, &issueDate);
    std::cout << "Код: " << slipCode
        << "\nТип: " << type
        << "\nСумма: " << amount
        << "\nДата: " << std::put_time(&timeStruct, "%d.%m.%Y %H:%M")
        << "\nСотрудник: " << (employee ? employee->getName() : "Не указан") << "\n"; // ← ДОБАВИТЬ << "\n"
}

std::istream& SalarySlip::read(std::istream& in) {
    bReadStr(slipCode, in);
    bReadStr(type, in);
    in.read((char*)&amount, sizeof(amount));
    in.read((char*)&issueDate, sizeof(issueDate));
    return in;
}

std::ostream& SalarySlip::write(std::ostream& out) {
    bWriteStr(slipCode, out);
    bWriteStr(type, out);
    out.write((char*)&amount, sizeof(amount));
    out.write((char*)&issueDate, sizeof(issueDate));
    return out;
}
void SalarySlip::setSession(std::shared_ptr<PayrollSession> newSession) {
    session = newSession;
}

std::shared_ptr<PayrollSession> SalarySlip::getSession() const {
    return session;
}
bool SalarySlip::operator<(const SalarySlip& other) const {
    return amount != other.amount ? amount < other.amount : slipCode < other.slipCode;
}

bool SalarySlip::operator==(const SalarySlip& other) const {
    return amount == other.amount && type == other.type;
}