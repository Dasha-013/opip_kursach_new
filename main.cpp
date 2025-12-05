#include <iostream>
#include <fstream>
#include <ctime>
#include <Windows.h>
#include <direct.h>
#include <memory>
#include "Base.h"
#include "Login.h"       
#include "Functions.h"   

std::streambuf* originalClogBuffer;
std::ofstream logFile;

void preSetup() {
    std::system("mode con cols=150 lines=40");

    logFile.open("log.txt");
    if (logFile.is_open()) {
        originalClogBuffer = std::clog.rdbuf();
        std::clog.rdbuf(logFile.rdbuf());
    }

    _mkdir("data");
    _mkdir("data\\users");

    std::srand(static_cast<unsigned>(std::time(0)));
}

void preExit() {
    if (logFile.is_open()) {
        std::clog.rdbuf(originalClogBuffer);
        logFile.close();
    }
}

int main() {
    Base* payrollSystem = nullptr;

    try {
        preSetup();

        GuestEmployeeBase base;
        base.createTestEmployees();

        LoginSystem loginSystem;
        payrollSystem = loginSystem.run();

        if (payrollSystem) {
            payrollSystem->run();

            if (auto employeeBase = dynamic_cast<EmployeeBase*>(payrollSystem)) {
                employeeBase->writeEmployees();  
            }

            delete payrollSystem;
        }

        preExit();
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        std::system("pause");
        return 1;
    }
    catch (...) {
        std::cerr << "Неизвестная критическая ошибка!" << std::endl;
        std::system("pause");
        return 1;
    }

    return 0;
}