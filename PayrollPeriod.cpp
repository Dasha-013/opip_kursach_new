#include <iomanip>  
#include "PayrollPeriod.h"
#include "Functions.h"
//Класс PayrollPeriod представляет собой временной интервал для расчета заработной платы,
// содержащий информацию о названии периода, его описании и продолжительности. 
PayrollPeriod::PayrollPeriod() {
    label = "UNDEFINED";
    description = "UNDEFINED";
}

PayrollPeriod::~PayrollPeriod() {
    sessions.clear();
}

std::ostream& operator<<(std::ostream& out, PayrollPeriod& period) {
    out << "Период: " << period.getLabel() << "\n"
        << "Длительность: " << period.getDuration().formatDate() << "\n"
        << "Описание: " << period.getDescription() << "\n";
    return out;
}

bool PayrollPeriod::operator!=(PayrollPeriod& other) {//разные периоды 
    return toLower(label) != toLower(other.label) || description != other.description;
}

bool PayrollPeriod::operator==(PayrollPeriod& other) {//одинаковые периоды 
    return toLower(label) == toLower(other.label) && description == other.description;
}

bool PayrollPeriod::operator<(PayrollPeriod& other) {//для сортирвоки(сравнение названия и описания)
    if (label != other.label)
        return toLower(label) < toLower(other.label);
    return description < other.description;
}

void PayrollPeriod::getInfo() {
    get("Введите название периода: ", label);
    get("Введите описание периода: ", description);

    std::cout << "Введите дату начала периода (dd.mm.yyyy): ";
    std::cin >> duration;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void PayrollPeriod::addSession(std::shared_ptr<PayrollSession> session) {
    sessions.insert(session);
}
std::ostream& PayrollPeriod::write(std::ostream& out) {
    bWriteStr(label, out);
    bWriteStr(description, out);
    out.write((char*)(&duration), sizeof(duration));
    int n = sessions.size();
    out.write((char*)(&n), sizeof(n));
    for (auto& session : sessions) {
        session->write(out);
    }
    return out;
}

std::istream& PayrollPeriod::read(std::istream& in) {
    bReadStr(label, in);
    bReadStr(description, in);
    in.read((char*)(&duration), sizeof(duration));
    int n = 0;
    in.read((char*)(&n), sizeof(n));
    for (int i = 0; i < n; ++i) {
        auto session = std::make_shared<PayrollSession>();
        session->read(in);
        sessions.insert(session);
    }
    return in;
}

std::string PayrollPeriod::getLabel() const {
    return label;
}

std::string PayrollPeriod::getDescription() const {
    return description;
}

SalaryDateTime PayrollPeriod::getDuration() const {
    return duration;
}

void PayrollPeriod::setLabel(const std::string& label) {
    this->label = label;
}

void PayrollPeriod::setDescription(const std::string& description) {
    this->description = description;
}

void PayrollPeriod::setDuration(const SalaryDateTime& dt) {
    this->duration = dt;
}

std::shared_ptr<PayrollSession> PayrollPeriod::chooseSession() {
    int n = 0;
    std::vector<std::shared_ptr<PayrollSession>> list;
    for (const auto& session : sessions) {
        std::cout << "[" << ++n << "]\n";
        std::cout << *session << "\n\n";
        list.push_back(session);
    }

    int ch;
    getInRange("Выберите номер сессии (0 - отмена): ", ch, 0, sessions.size());
    if (ch == 0) {
        std::cout << "Отмена...\n";
        return nullptr;
    }
    ch--;
    std::cout << "Вы выбрали:\n" << *list[ch] << "\n";
    return list[ch];
}

void PayrollPeriod::showSessions() const {
    for (const auto& session : sessions) {
        std::cout << *session << "\n";
    }
}

bool payrollPeriodCmp::operator()(const std::shared_ptr<PayrollPeriod>& p1, const std::shared_ptr<PayrollPeriod>& p2) const {//сравнение умных указателей 
    return (*p1) < (*p2);
}
