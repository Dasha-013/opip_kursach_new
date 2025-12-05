#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>

void get(const std::string& prompt, std::string& value);
void get(const std::string& prompt, int& value);
void get(const std::string& prompt, double& value);

void getInRange(const std::string& prompt, int& value, int min, int max);//проверка диапозона 
std::string toLower(const std::string& s);
bool checkInpFile(std::ifstream& file);//проверка открытия файла 


template<class T>
void promptValue(const std::string& message, T& value) {//обработка ошибок 
    std::cout << message;
    std::cin >> value;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(100, '\n');
        std::cout << "Неверный ввод. Повторите попытку: ";
        std::cin >> value;
    }
    if (std::cin.bad()) {
        std::clog << "Критическая ошибка ввода!";
        throw std::exception("INPUT ERROR!!!");
    }
}

void bWriteStr(const std::string& s, std::ostream& out);
void bReadStr(std::string& s, std::istream& in);

