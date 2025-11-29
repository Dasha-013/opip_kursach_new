#include "Base.h"
#include "Functions.h"
#include "PayrollSession.h"
#include "PayrollPeriod.h"
#include "SalaryDocumentation.h"
#include "EmployeeUser.h"
#include "SalaryDateTime.h"
#include "TableFormatter.h"

// EmployeeBase implementation
EmployeeBase::EmployeeBase() {
    readEmployees();
}

EmployeeBase::~EmployeeBase() {
    writeEmployees();
}

void EmployeeBase::readEmployees() {
    std::ifstream in("data\\employees.bin", std::ios::binary);

    if (!checkInpFile(in)) {
        std::cout << "Файл сотрудников не найден или пуст. Созданы тестовые сотрудники.\n";
        createTestEmployees();
        return;
    }

    try {
        int size = 0;
        in.read((char*)(&size), sizeof(size));
        if (size <= 0) {
            std::cout << "Файл сотрудников пуст. Созданы тестовые сотрудники.\n";
            createTestEmployees();
            return;
        }

        employeesRepo.clear();
        for (int i = 0; i < size; ++i) {
            int employeeType;
            in.read((char*)(&employeeType), sizeof(employeeType));
            std::shared_ptr<Employee> emp;
            switch (employeeType) {
            case 1: emp = std::make_shared<RegularEmployee>(); break;
            case 2: emp = std::make_shared<ManagerEmployee>(); break;
            case 3: emp = std::make_shared<SalesEmployee>(); break;
            case 4: emp = std::make_shared<HourlyEmployee>(); break;
            case 5: emp = std::make_shared<BonusEmployee>(); break;
            default:
                std::cout << "Неизвестный тип сотрудника: " << employeeType << std::endl;
                continue;
            }
            emp->read(in);
            employeesRepo.add(emp);
        }
        std::cout << "Загружено сотрудников: " << employeesRepo.size() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка при чтении файла сотрудников: " << e.what() << "\n";
        std::cout << "Созданы тестовые сотрудники.\n";
        createTestEmployees();
    }
    in.close();
}
void EmployeeBase::showAll() {
    if (employeesRepo.empty()) {
        std::cout << "Список сотрудников пуст!\n";
        return;
    }

    std::cout << "=== СПИСОК ВСЕХ СОТРУДНИКОВ ===\n";
    std::cout << "Всего сотрудников: " << employeesRepo.size() << "\n\n";

    int counter = 1;
    employeesRepo.forEach([&](auto emp) {
        std::cout << "--- [" << counter++ << "] -------------------------\n";
        std::cout << "ФИО: " << emp->getName() << "\n"
            << "Должность: " << emp->getPosition() << "\n"
            << "Зарплата: " << emp->calculateSalary() << " руб.\n";
        });
    std::cout << "----------------------------------------\n";
}

std::shared_ptr<Employee> EmployeeBase::chooseEmployee() {
    if (employeesRepo.empty()) {
        std::cout << "Список сотрудников пуст!\n";
        system("pause");
        return nullptr;
    }

    auto list = employeesRepo.getAll();

    std::cout << "=== ВЫБОР СОТРУДНИКА ===\n";
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << list[i]->getName()
            << " — " << list[i]->getPosition()
            << " (" << list[i]->calculateSalary() << " руб.)\n";
    }

    int ch;
    getInRange("\nВыберите сотрудника (0 - отмена): ", ch, 0, list.size());

    if (ch == 0) {
        std::cout << "Выбор отменен.\n";
        return nullptr;
    }

    auto selected = list[ch - 1];
    std::cout << "Выбран: " << selected->getName() << "\n";
    return selected;
}

void EmployeeBase::writeEmployees() {
    // Создаем папку data если её нет
    system("mkdir data 2>nul");

    std::ofstream out("data\\employees.bin", std::ios::binary);
    int size = employeesRepo.size();
    out.write((char*)(&size), sizeof(size));

    employeesRepo.forEach([&](auto emp) {
        // Записываем тип сотрудника
        int employeeType = 0;

        if (dynamic_cast<RegularEmployee*>(emp.get())) employeeType = 1;
        else if (dynamic_cast<ManagerEmployee*>(emp.get())) employeeType = 2;
        else if (dynamic_cast<SalesEmployee*>(emp.get())) employeeType = 3;
        else if (dynamic_cast<HourlyEmployee*>(emp.get())) employeeType = 4;
        else if (dynamic_cast<BonusEmployee*>(emp.get())) employeeType = 5;

        out.write((char*)(&employeeType), sizeof(employeeType));
        emp->write(out);
        });
    out.close();
}

void EmployeeBase::createTestEmployees() {
    employeesRepo.clear();

    auto emp1 = std::make_shared<ManagerEmployee>("Богля Кирилл", 50000, 0.2);
    auto emp2 = std::make_shared<SalesEmployee>("Суднека Ульяна", 30000, 150000, 0.1);
    auto emp3 = std::make_shared<HourlyEmployee>("Скридлевская Анна", 160, 500);
    auto emp4 = std::make_shared<BonusEmployee>("Голик Лиза", "Переводчик", 100000, 10000);
    auto emp5 = std::make_shared<RegularEmployee>("Ращеня Рома", "Программист", 90000);
    auto emp6 = std::make_shared<ManagerEmployee>("Васько Алёна", 80000, 0.25);
    auto emp7 = std::make_shared<SalesEmployee>("Иванчик Дарья", 25000, 200000, 0.15);
    auto emp8 = std::make_shared<HourlyEmployee>("Андриевич Вика", 120, 700);
    auto emp9 = std::make_shared<BonusEmployee>("Семенов Алексей", "Аналитик", 75000, 15000);
    auto emp10 = std::make_shared<RegularEmployee>("Петрова Мария", "Дизайнер", 60000);
    auto emp11 = std::make_shared<SalesEmployee>("Козлов Дмитрий", 35000, 50000, 0.05);
    auto emp12 = std::make_shared<HourlyEmployee>("Новикова Елена", 80, 400);
    auto emp13 = std::make_shared<BonusEmployee>("Федоров Иван", "Тестировщик", 45000, 5000);
    auto emp14 = std::make_shared<ManagerEmployee>("Смирнова Ольга", 60000, 0.15);
    auto emp15 = std::make_shared<RegularEmployee>("Кузнецов Артем", "Разработчик", 95000);

    employeesRepo.add(emp1);
    employeesRepo.add(emp2);
    employeesRepo.add(emp3);
    employeesRepo.add(emp4);
    employeesRepo.add(emp5);
    employeesRepo.add(emp6);
    employeesRepo.add(emp7);
    employeesRepo.add(emp8);
    employeesRepo.add(emp9);
    employeesRepo.add(emp10);
    employeesRepo.add(emp11);
    employeesRepo.add(emp12);
    employeesRepo.add(emp13);
    employeesRepo.add(emp14);
    employeesRepo.add(emp15);

    writeEmployees();
    std::cout << "Тестовые сотрудники созданы!\n";
}

// GuestEmployeeBase implementation
void GuestEmployeeBase::showHeader() {
    std::cout << "Добро пожаловать, гость!" << std::endl;
}

void GuestEmployeeBase::run() {
    int ch = 0;
    do {
        system("CLS");
        showHeader();
        std::cout << "                         ----------------" << std::endl
            << "                         | Гостевой режим |" << std::endl
            << "                         ----------------" << std::endl
            << "[1] Просмотреть сотрудников." << std::endl
            << "[2] Выбрать сотрудника." << std::endl
            << "[0] Выход." << std::endl;
        getInRange("Ваш выбор: ", ch, 0, 2);
        switch (ch) {
        case 1:
            showAll();
            system("pause");
            break;
        case 2:
            employeeMenu(chooseEmployee());
            break;
        }
    } while (ch != 0);
}

void GuestEmployeeBase::employeeMenu(std::shared_ptr<Employee> emp) {
    if (!emp) {
        std::cout << "Сотрудник не выбран!\n";
        system("pause");
        return;
    }

    int ch = 0;
    do {
        system("CLS");
        std::cout << "============== ИНФОРМАЦИЯ О СОТРУДНИКЕ ==============\n" << std::endl
            << "ФИО: " << emp->getName() << std::endl
            << "Должность: " << emp->getPosition() << std::endl
            << "Расчетная зарплата: " << emp->calculateSalary() << " руб." << std::endl
            << "=====================================================" << std::endl
            << "[1] Показать детальную информацию" << std::endl
            << "[0] Назад" << std::endl;
        getInRange("Ваш выбор: ", ch, 0, 1);

        if (ch == 1) {
            std::cout << "\n=== ДЕТАЛЬНАЯ ИНФОРМАЦИЯ ===\n";
            emp->showDetailedInfo();
            system("pause");
        }
    } while (ch != 0);
}

// UserPayrollBase implementation
UserPayrollBase::UserPayrollBase(std::shared_ptr<EmployeeUser> u) : user(u) {
    loadUserPeriods();
}

void UserPayrollBase::showHeader() {
    std::cout << "========================================\n";
    std::cout << "           ЛИЧНЫЙ КАБИНЕТ\n";
    std::cout << "========================================\n";
}

void UserPayrollBase::showHead() {
    std::cout << "Добро пожаловать, " << user->getLogin() << "!" << std::endl << std::endl;
}

void UserPayrollBase::run() {
    int ch = 0;
    do {
        system("CLS");
        showHead();
        std::cout << "                         ----------------" << std::endl
            << "                         | Личный кабинет |" << std::endl
            << "                         ----------------" << std::endl
            << "[1] Просмотреть периоды расчёта." << std::endl
            << "[2] Выбрать период." << std::endl
            << "[3] Просмотреть свои ведомости." << std::endl
            << "[4] Просмотреть личные данные." << std::endl
            << "[5] Редактировать личные данные." << std::endl
            << "[0] Выход." << std::endl;
        getInRange("Ваш выбор: ", ch, 0, 5);
        switch (ch) {
        case 1:
            showPeriods();
            system("pause");
            break;
        case 2:
            periodMenu(choosePeriod());
            break;
        case 3:
            user->showSlips();
            system("pause");
            break;
        case 4:
            user->showInfo();
            system("pause");
            break;
        case 5:
            std::cout << "Введите новые данные:\n";
            user->getInfo();
            user->writeInfo();
            std::cout << "Данные обновлены и сохранены!\n";
            system("pause");
            break;
        }
    } while (ch != 0);
}

void UserPayrollBase::loadUserPeriods() {
    userPeriods.clear();

    // Данные для всех 12 месяцев: [название, последний день, аванс, зарплата]
    std::vector<std::tuple<std::string, int, double, double>> monthsData = {
        {"Январь", 31, 30000, 45000},
        {"Февраль", 29, 32000, 53000},
        {"Март", 31, 30000, 45000},
        {"Апрель", 30, 35000, 52500},
        {"Май", 31, 35000, 58000},
        {"Июнь", 30, 35000, 52500},
        {"Июль", 31, 28000, 42000},
        {"Август", 31, 35000, 52500},
        {"Сентябрь", 30, 35000, 60000},
        {"Октябрь", 31, 35000, 52500},
        {"Ноябрь", 30, 35000, 52500},
        {"Декабрь", 31, 35000, 70000}
    };

    for (int month = 0; month < 12; ++month) {
        const auto& monthData = monthsData[month];
        std::string monthName = std::get<0>(monthData);
        int lastDay = std::get<1>(monthData);
        double advanceAmount = std::get<2>(monthData);
        double salaryAmount = std::get<3>(monthData);

        auto period = std::make_shared<PayrollPeriod>();
        period->setLabel(monthName + " 2024");
        period->setDescription("Расчет зарплаты за " + monthName + " 2024");

        SalaryDateTime advanceDate, salaryDate;

        try {
            advanceDate.setDate(2024, month + 1, 15, 10, 0);
            salaryDate.setDate(2024, month + 1, lastDay, 15, 0);
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка установки даты для " << monthName << ": " << e.what() << std::endl;
            continue;
        }

        auto advanceSession = std::make_shared<PayrollSession>(
            "Аванс " + monthName + " 2024", advanceDate);

        auto salarySession = std::make_shared<PayrollSession>(
            "Зарплата " + monthName + " 2024", salaryDate);

        period->addSession(advanceSession);
        period->addSession(salarySession);

        if (user) {
            auto advanceSlip = std::make_shared<SalarySlip>(advanceAmount, "Аванс", nullptr, static_cast<std::time_t>(advanceDate));
            auto salarySlip = std::make_shared<SalarySlip>(salaryAmount, "Зарплата", nullptr, static_cast<std::time_t>(salaryDate));

            advanceSlip->setSession(advanceSession);
            salarySlip->setSession(salarySession);

            advanceSession->addSlip(advanceSlip);
            salarySession->addSlip(salarySlip);

            user->pushSlip(advanceSlip);
            user->pushSlip(salarySlip);
        }

        userPeriods.push_back(period);
    }

    std::cout << "Загружено периодов: " << userPeriods.size() << " месяцев\n";
}

void UserPayrollBase::showPeriods() {
    if (userPeriods.empty()) {
        std::cout << "У вас нет расчетных периодов.\n";
        return;
    }

    std::cout << "=== ВАШИ РАСЧЕТНЫЕ ПЕРИОДЫ ===\n";
    std::cout << "----------------------------------------\n";

    forEachUserPeriod([&](auto period) {
        std::cout << "Период: " << period->getLabel() << "\n";
        std::cout << "    Описание: " << period->getDescription() << "\n";
        std::cout << "    Дата: " << period->getDuration() << "\n";
        std::cout << "----------------------------------------\n";
        });

    std::cout << "Всего периодов: " << userPeriods.size() << "\n";
}

std::shared_ptr<PayrollPeriod> UserPayrollBase::choosePeriod() {
    if (userPeriods.empty()) {
        std::cout << "У вас нет расчетных периодов.\n";
        return nullptr;
    }

    std::cout << "=== ВЫБОР РАСЧЕТНОГО ПЕРИОДА ===\n";

    for (size_t i = 0; i < userPeriods.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << userPeriods[i]->getLabel() << "\n";
        std::cout << "    " << userPeriods[i]->getDescription() << "\n";
        std::cout << "----------------------------------------\n";
    }

    int choice;
    getInRange("Выберите период (0 - отмена): ", choice, 0, userPeriods.size());

    if (choice == 0) {
        std::cout << "Выбор отменен.\n";
        return nullptr;
    }

    auto selectedPeriod = userPeriods[choice - 1];
    std::cout << "Выбран период: " << selectedPeriod->getLabel() << "\n";
    return selectedPeriod;
}

void UserPayrollBase::periodMenu(std::shared_ptr<PayrollPeriod> period) {
    if (!period) return;

    int ch = 0;
    do {
        system("CLS");
        std::cout << "=== ВАШ РАСЧЕТНЫЙ ПЕРИОД ===\n"
            << "Период: " << period->getLabel() << "\n"
            << "Описание: " << period->getDescription() << "\n"
            << "Дата: " << period->getDuration() << "\n"
            << "===================================\n"
            << "[1] Детальная информация по выплатам\n"
            << "[0] Назад\n";
        getInRange("Ваш выбор: ", ch, 0, 1);

        switch (ch) {
        case 1:
            sessionMenu(period->chooseSession());
            break;
        }
    } while (ch != 0);
}

void UserPayrollBase::sessionMenu(std::shared_ptr<PayrollSession> session) {
    if (!session) return;

    int ch = 0;
    do {
        system("CLS");
        std::cout << "=== ИНФОРМАЦИЯ О ВЫПЛАТАХ ===\n"
            << "Сессия: " << session->getPeriodLabel() << "\n"
            << "Дата: " << session->getDate() << "\n"
            << "===================================\n"
            << "[1] Детальная информация по ведомости\n"
            << "[0] Назад\n";
        getInRange("Ваш выбор: ", ch, 0, 1);

        switch (ch) {
        case 1:
            slipMenu(session->chooseSlip());
            break;
        }
    } while (ch != 0);
}

void UserPayrollBase::slipMenu(std::shared_ptr<SalarySlip> slip) {
    if (!slip) return;
    int ch = 0;
    do {
        system("CLS");
        TableFormatter::displayDetailedSalarySlip(slip);
        std::cout << "\n══════════════════════════════════════════════════════════════════════\n"
            << "[0] Назад.\n";
        getInRange("Ваш выбор: ", ch, 0, 0);
    } while (ch != 0);
}

// AdminEmployeeBase implementation
int AdminEmployeeBase::adminInstanceCount = 0;

AdminEmployeeBase::AdminEmployeeBase(std::shared_ptr<EmployeeUser> u) : adminUser(u) {
    adminInstanceCount++;

    std::cout << "=== СИСТЕМНЫЙ СТАТУС ===\n";
    std::cout << "Администратор " << adminUser->getLogin() << " вошел в систему.\n";
    std::cout << "Активных администраторов: " << adminInstanceCount << "\n";
    std::cout << "=========================\n";
    system("pause");
}

AdminEmployeeBase::~AdminEmployeeBase() {
    adminInstanceCount--;

    std::cout << "=== СИСТЕМНЫЙ СТАТУС ===\n";
    std::cout << "Администратор " << (adminUser ? adminUser->getLogin() : "unknown")
        << " вышел из системы.\n";
    std::cout << "Осталось активных администраторов: " << adminInstanceCount << "\n";
    std::cout << "=========================\n";
}

int AdminEmployeeBase::getActiveAdminCount() {
    return adminInstanceCount;
}

void AdminEmployeeBase::showHeader() {
    std::cout << "========================================\n";
    std::cout << "        АДМИНИСТРАТИВНАЯ ПАНЕЛЬ\n";
    std::cout << "========================================\n";
}

void AdminEmployeeBase::showHead() {
    std::cout << "Добро пожаловать, администратор " << adminUser->getLogin() << "!\n\n";
    std::cout << "Активных администраторов в системе: " << getActiveAdminCount() << "\n\n";
}

void AdminEmployeeBase::run() {
    int ch = 0;
    do {
        system("CLS");
        showHead();
        std::cout << "                         ----------------\n"
            << "                         | Панель Админа |\n"
            << "                         ----------------\n"
            << "[1] Просмотреть периоды расчёта.\n"
            << "[2] Добавить период.\n"
            << "[3] Выбрать период.\n"
            << "[4] Удалить период (через стек).\n"
            << "[5] Просмотреть удаленные периоды.\n"
            << "[6] Отменить удаление периода.\n"
            << "[7] Обработать очередь удаления.\n"
            << "[8] Просмотреть системную информацию.\n"
            << "[9] Просмотреть личные данные.\n"
            << "[10] Редактировать личные данные.\n"
            << "[11] Редактировать сотрудника\n"
            << "[0] Выход.\n";
        getInRange("Ваш выбор: ", ch, 0, 11);

        switch (ch) {
        case 1:
            showPeriods();
            system("pause");
            break;
        case 2:
            addPeriod();
            std::cout << "Период успешно добавлен!\n";
            system("pause");
            break;
        case 3:
            periodMenu(choosePeriod());
            break;
        case 4:
            deletePeriodWithStack();
            system("pause");
            break;
        case 5:
            showDeletedPeriods();
            system("pause");
            break;
        case 6:
            undoDelete();
            system("pause");
            break;
        case 7:
            processDeletionQueue();
            system("pause");
            break;
        case 8:
            showSystemInfo();
            system("pause");
            break;
        case 9:
            adminUser->showInfo();
            system("pause");
            break;
        case 10:
            std::cout << "Введите новые данные:\n";
            adminUser->getInfo();
            adminUser->writeInfo();
            std::cout << "Данные обновлены и сохранены!\n";
            system("pause");
            break;
        case 11: {
            if (employeesRepo.empty()) {
                std::cout << "Список сотрудников пуст!\n";
                system("pause");
                break;
            }
            auto emp = chooseEmployee();
            if (!emp) break;
            std::cout << "Редактирование сотрудника: " << emp->getName() << "\n";
            emp->edit();
            writeEmployees();
            std::cout << "Данные сотрудника обновлены и сохранены!\n";
            system("pause");
            break;
        }
        }
    } while (ch != 0);
}

void AdminEmployeeBase::showSystemInfo() {
    std::cout << "=== СИСТЕМНАЯ ИНФОРМАЦИЯ ===\n";
    std::cout << "Текущий администратор: " << adminUser->getLogin() << "\n";
    std::cout << "Активных администраторов: " << getActiveAdminCount() << "\n";
    std::cout << "Всего сотрудников в базе: " << employeesRepo.size() << "\n";
    std::cout << "Активных периодов: " << periodsRepo.size() << "\n";
    std::cout << "Периодов в стеке удаления: " << deletedPeriodsStack.size() << "\n";
    std::cout << "Периодов в очереди удаления: " << deletionQueue.size() << "\n";
    std::cout << "=============================\n";

    if (getActiveAdminCount() > 1) {
        std::cout << " ВНИМАНИЕ: В системе работает несколько администраторов!\n";
    }
    if (deletionQueue.size() > 5) {
        std::cout << " ВНИМАНИЕ: Большая очередь удаления (" << deletionQueue.size()
            << " периодов)\n";
    }
    std::cout << "=============================\n";
}

void AdminEmployeeBase::deletePeriodWithStack() {
    if (periodsRepo.empty()) {
        std::cout << "Нет доступных периодов для удаления.\n";
        return;
    }

    std::cout << "=== ВЫБЕРИТЕ ПЕРИОД ДЛЯ УДАЛЕНИЯ ===\n";
    auto periodList = periodsRepo.getAll();

    for (size_t i = 0; i < periodList.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << periodList[i]->getLabel()
            << " - " << periodList[i]->getDescription() << "\n";
    }

    int choice;
    getInRange("Выберите период для удаления (0 - отмена): ", choice, 0, periodList.size());

    if (choice == 0) {
        std::cout << "Удаление отменено.\n";
        return;
    }

    auto periodToDelete = periodList[choice - 1];
    periodsRepo.remove(periodToDelete);
    deletedPeriodsStack.push(periodToDelete);
    deletionQueue.push(periodToDelete);

    std::cout << "Период \"" << periodToDelete->getLabel() << "\" успешно удален!\n";
    std::cout << "Период помещен в стек удаления. Размер стека: "
        << deletedPeriodsStack.size() << "\n";
    std::cout << "Используйте 'Отменить удаление периода' для восстановления.\n";
}

void AdminEmployeeBase::showDeletedPeriods() {
    if (deletedPeriodsStack.empty()) {
        std::cout << "Стек удаленных периодов пуст.\n";
        return;
    }

    std::cout << "=== УДАЛЕННЫЕ ПЕРИОДЫ (в порядке отмены) ===\n";
    std::stack<std::shared_ptr<PayrollPeriod>> tempStack = deletedPeriodsStack;
    int counter = 1;

    while (!tempStack.empty()) {
        auto period = tempStack.top();
        std::cout << "[" << counter++ << "] " << period->getLabel()
            << " - " << period->getDescription() << "\n";
        tempStack.pop();
    }

    std::cout << "Всего удаленных периодов: " << deletedPeriodsStack.size() << "\n";
}

void AdminEmployeeBase::undoDelete() {
    if (deletedPeriodsStack.empty()) {
        std::cout << "Нет удаленных периодов для восстановления.\n";
        return;
    }

    auto period = deletedPeriodsStack.top();
    deletedPeriodsStack.pop();

    std::queue<std::shared_ptr<PayrollPeriod>> tempQueue;
    bool found = false;

    while (!deletionQueue.empty()) {
        auto current = deletionQueue.front();
        deletionQueue.pop();

        if (current == period && !found) {
            found = true;
            std::cout << "Период \"" << period->getLabel() << "\" удален из очереди удаления.\n";
        }
        else {
            tempQueue.push(current);
        }
    }

    deletionQueue = tempQueue;
    periodsRepo.add(period);
    std::cout << "Период \"" << period->getLabel() << "\" восстановлен!\n";
    std::cout << "В стеке осталось периодов: " << deletedPeriodsStack.size() << "\n";
    std::cout << "В очереди удаления осталось периодов: " << deletionQueue.size() << "\n";
}

void AdminEmployeeBase::processDeletionQueue() {
    if (deletionQueue.empty()) {
        std::cout << "Очередь удаления пуста.\n";
        return;
    }

    std::cout << "=== ОКОНЧАТЕЛЬНОЕ УДАЛЕНИЕ ПЕРИОДОВ ===\n";
    int count = 0;

    while (!deletionQueue.empty()) {
        auto period = deletionQueue.front();
        deletionQueue.pop();
        std::cout << "Период \"" << period->getLabel() << "\" окончательно удален.\n";
        count++;
    }

    std::cout << "Обработано периодов: " << count << "\n";
    std::cout << "Очередь удаления очищена.\n";

    while (!deletedPeriodsStack.empty()) {
        deletedPeriodsStack.pop();
    }
    std::cout << "Стек удаления также очищен.\n";
}

void AdminEmployeeBase::showPeriods() {
    if (periodsRepo.empty()) {
        std::cout << "Нет доступных периодов.\n";
        return;
    }

    std::cout << "=== ДОСТУПНЫЕ ПЕРИОДЫ ===\n";
    forEachPeriod([&](auto p) {
        std::cout << *p << "\n";
        });

    std::cout << "=== СИСТЕМА УДАЛЕНИЯ ===\n";
    std::cout << "Периодов в стеке отмены: " << deletedPeriodsStack.size() << "\n";
    std::cout << "Периодов в очереди удаления: " << deletionQueue.size() << "\n";

    if (!deletedPeriodsStack.empty()) {
        std::cout << "Последний удаленный период: "
            << deletedPeriodsStack.top()->getLabel() << "\n";
    }
    std::cout << "Активных администраторов: " << getActiveAdminCount() << "\n";
    std::cout << "========================\n";
}

void AdminEmployeeBase::addPeriod() {
    auto period = std::make_shared<PayrollPeriod>();
    period->getInfo();

    SalaryDateTime periodStart = period->getDuration();
    int year = periodStart.getYear();
    int month = periodStart.getMonth();

    auto advanceSession = std::make_shared<PayrollSession>();
    advanceSession->setID(1);
    advanceSession->setPeriodLabel("Аванс " + period->getLabel());

    SalaryDateTime advanceDate;
    advanceDate.setDate(year, month, 15, 10, 0);
    advanceSession->setDate(advanceDate);
    advanceSession->generateSlips(employeeUsers, 0);

    auto salarySession = std::make_shared<PayrollSession>();
    salarySession->setID(2);
    salarySession->setPeriodLabel("Зарплата " + period->getLabel());

    SalaryDateTime salaryDate;
    int lastDay = SalaryDateTime::getLastDayOfMonth(year, month);
    salaryDate.setDate(year, month, lastDay, 15, 0);
    salarySession->setDate(salaryDate);
    salarySession->generateSlips(employeeUsers, 0);

    period->addSession(advanceSession);
    period->addSession(salarySession);
    periodsRepo.add(period);

    std::cout << "Период \"" << period->getLabel() << "\" создан!\n";
    system("pause");
}

void AdminEmployeeBase::periodMenu(std::shared_ptr<PayrollPeriod> period) {
    if (!period) return;
    int ch = 0;
    do {
        system("CLS");
        std::cout << "_____________________ Период _____________________\n"
            << *period << "\n"
            << "__________________________________________________\n"
            << "[1] Показать сессии.\n"
            << "[2] Добавить сессию.\n"
            << "[3] Выбрать сессию.\n"
            << "[4] Редактировать информацию.\n"
            << "[0] Назад.\n";
        getInRange("Ваш выбор: ", ch, 0, 4);
        switch (ch) {
        case 1:
            period->showSessions();
            system("pause");
            break;
        case 2: {
            auto session = std::make_shared<PayrollSession>();
            SalaryDateTime periodStart = period->getDuration();
            int year = periodStart.getYear();
            int month = periodStart.getMonth();

            std::string sessionType;
            SalaryDateTime paymentDate;

            int sessionsCount = period->getSessionsCount();

            if (sessionsCount % 2 == 0) {
                sessionType = "Аванс";
                paymentDate.setDate(year, month, 15, 10, 0);
            }
            else {
                sessionType = "Зарплата";
                int lastDay = SalaryDateTime::getLastDayOfMonth(year, month);
                paymentDate.setDate(year, month, lastDay, 15, 0);
            }

            session->setID(sessionsCount + 1);
            session->setPeriodLabel(sessionType + " " + period->getLabel());
            session->setDate(paymentDate);
            session->generateSlips(employeeUsers, 0);

            period->addSession(session);
            std::cout << sessionType << " \"" << session->getPeriodLabel()
                << "\" на дату " << paymentDate << " добавлена!\n";
            system("pause");
            break;
        }
        case 3:
            sessionMenu(period->chooseSession());
            break;
        case 4:
            period->getInfo();
            std::cout << "Данные периода обновлены!\n";
            system("pause");
            break;
        }
    } while (ch != 0);
}

void AdminEmployeeBase::sessionMenu(std::shared_ptr<PayrollSession> session) {
    if (!session) return;
    int ch = 0;
    do {
        system("CLS");
        std::cout << "_____________________ Сессия _____________________\n"
            << *session << "\n"
            << "__________________________________________________\n"
            << "[1] Показать ведомости.\n"
            << "[2] Сгенерировать ведомости.\n"
            << "[3] Выбрать ведомость.\n"
            << "[4] Редактировать сессию.\n"
            << "[0] Назад.\n";
        getInRange("Ваш выбор: ", ch, 0, 4);
        switch (ch) {
        case 1:
            session->showSlips();
            system("pause");
            break;
        case 2: {
            std::cout << "Создание новых ведомостей:\n";

            if (employeesRepo.empty()) {
                std::cout << "Нет сотрудников для создания ведомостей!\n";
            }
            else {
                employeesRepo.forEach([&](auto emp) {
                    double salary = emp->calculateSalary();
                    auto slip = std::make_shared<SalarySlip>(salary, "Расчет", emp, static_cast<std::time_t>(session->getDate()));
                    slip->setSession(session);
                    session->addSlip(slip);
                    });
                std::cout << "Создано ведомостей: " << employeesRepo.size() << "\n";
            }

            system("pause");
            break;
        }
        case 3:
            slipMenu(session->chooseSlip());
            break;
        case 4: {
            SalaryDateTime date;
            std::string label;
            std::cout << "Введите новую дату (dd.mm.yyyy): ";
            std::cin >> date;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            get("Введите новый заголовок периода: ", label);
            session->setDate(date);
            session->setPeriodLabel(label);
            session->setDate(std::time(nullptr));
            break;
        }
        }
    } while (ch != 0);
}

void AdminEmployeeBase::slipMenu(std::shared_ptr<SalarySlip> slip) {
    if (!slip) return;
    int ch = 0;
    do {
        system("CLS");
        TableFormatter::displayDetailedSalarySlip(slip);

        std::cout << "\n══════════════════════════════════════════════════════════════════════\n"
            << "[1] Изменить сумму выплаты.\n"
            << "[0] Назад.\n";
        getInRange("Ваш выбор: ", ch, 0, 1);
        switch (ch) {
        case 1: {
            double amount;
            get("Введите новую сумму: ", amount);
            while (amount < 0) {
                get("Сумма не может быть отрицательной. Повторите: ", amount);
            }
            slip->setAmount(amount);
            std::cout << "Сумма обновлена!\n";
            system("pause");
            break;
        }
        }
    } while (ch != 0);
}

std::shared_ptr<PayrollPeriod> AdminEmployeeBase::choosePeriod() {
    if (periodsRepo.empty()) {
        std::cout << "Нет созданных расчетных периодов.\n";
        std::cout << "Создайте новый период через меню.\n";
        return nullptr;
    }

    std::cout << "Список расчетных периодов:\n";
    std::cout << "----------------------------------------\n";

    auto periodList = periodsRepo.getAll();
    for (size_t i = 0; i < periodList.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << periodList[i]->getLabel() << "\n";
        std::cout << "    Описание: " << periodList[i]->getDescription() << "\n";
        std::cout << "----------------------------------------\n";
    }

    int choice;
    getInRange("Выберите период для управления (0 - отмена): ", choice, 0, periodList.size());
    if (choice == 0) {
        std::cout << "Выбор отменен.\n";
        return nullptr;
    }

    auto selectedPeriod = periodList[choice - 1];
    std::cout << "Выбран период: " << selectedPeriod->getLabel() << "\n";
    return selectedPeriod;
}
GuestEmployeeBase::GuestEmployeeBase() {
    readEmployees();
}

// AdminEmployeeBase::employeeMenu
void AdminEmployeeBase::employeeMenu(std::shared_ptr<Employee> emp) {
    // Пустая реализация
}

// UserPayrollBase::employeeMenu  
void UserPayrollBase::employeeMenu(std::shared_ptr<Employee> emp) {
    // Пустая реализация
}