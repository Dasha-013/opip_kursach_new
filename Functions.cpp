#include "Functions.h"
#include <direct.h>
#include <cerrno>
//обработка исключений, корректный ввод 
void get(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    if (std::cin.peek() == '\n') std::cin.ignore(100, '\n');
    std::getline(std::cin, value);
    while (std::cin.fail()) {
        std::cin.clear();
        std::cout << "Ошибка ввода. Повторите: ";
        std::getline(std::cin, value);
    }
    if (std::cin.bad()) {
        std::clog << "Критическая ошибка ввода!";
        throw std::exception("INPUT ERROR");
    }
}

void get(const std::string& prompt, int& value) {
    std::cout << prompt;
    std::cin >> value;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(100, '\n');
        std::cout << "Ошибка ввода. Повторите: ";
        std::cin >> value;
    }
    if (std::cin.bad()) {
        std::clog << "Критическая ошибка ввода!";
        throw std::exception("INPUT ERROR");
    }
}
void get(const std::string& prompt, double& value) {
    std::cout << prompt;
    std::cin >> value;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(100, '\n');
        std::cout << "Ошибка ввода. Повторите: ";
        std::cin >> value;
    }
    if (std::cin.bad()) {
        std::clog << "Критическая ошибка ввода!";
        throw std::exception("INPUT ERROR");
    }
}

void getInRange(const std::string& prompt, int& value, int min, int max) {
    get(prompt, value);
    while (value < min || value > max) {
        std::cout << "Значение должно быть от " << min << " до " << max << "!\n";
        get(prompt, value);
    }
}

std::string toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        if (c >= 'А' && c <= 'Я') c += 'а' - 'А';
        else if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    }
    return result;
}

void bWriteStr(const std::string& s, std::ostream& out) {
    int len = s.length();
    out.write((char*)&len, sizeof(int));
    out.write(s.data(), len);
}

void bReadStr(std::string& s, std::istream& in) {
    int len;
    in.read((char*)&len, sizeof(int));
    char* buffer = new char[len + 1];
    in.read(buffer, len);
    buffer[len] = '\0';
    s = buffer;
    delete[] buffer;
}


bool checkInpFile(std::ifstream& f) {
    if (f.bad()) {
        std::clog << "Критическая ошибка потока: поток поврежден.\n";
        return false;
    }
    if (f.fail()) {
        std::clog << "Ошибка логической операции с файлом.\n";
        return false;
    }
    if (!f.is_open()) {
        std::clog << "Файл не открыт.\n";
        return false;
    }
    std::streampos currentPos = f.tellg();

    f.seekg(0, std::ios::end);
    if (f.fail()) {
        std::clog << "Ошибка позиционирования в файле.\n";
        f.clear(); 
        f.seekg(currentPos);
        return false;
    }

    if (f.tellg() == 0) {
        std::clog << "Файл пуст.\n";
        f.seekg(currentPos); 
        return false;
    }

    f.seekg(0, std::ios::beg);
    if (f.fail()) {
        std::clog << "Ошибка возврата к началу файла.\n";
        f.clear();
        return false;
    }
    if (!f.good()) {
        std::clog << "Поток в неопределенном состоянии.\n";
        return false;
    }

    return true;
}

