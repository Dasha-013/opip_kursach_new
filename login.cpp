#include "Login.h"
#include "Functions.h"
#include "EmployeeUser.h"
#include "Base.h"
#include <Windows.h>
#include <conio.h>
#include <fstream>
#include <functional>
#include <random>
using namespace std;
//пользователь может войти под существующей учетной записью, зарегистрироваться как новый 
// пользователь или войти в гостевом режиме. При регистрации система проверяет уникальность логина, 
// запрашивает подтверждение пароля с скрытым вводом через звездочки, генерирует криптографическую соль и хеширует пароль для безопасного хранения.
//роли : админ гость пользователь(сотрудник)
void getPassword(const string& prompt, string& password) {
    cout << prompt;
    password = "";
    char ch;
    while ((ch = _getch()) != 13) {
        if (ch == 8 && !password.empty()) {
            password.pop_back();
            cout << "\b \b";
        }
        else if (ch != 8) {
            password += ch;
            _putch('*');
        }
    }
    cout << endl;
}

LoginSystem::LoginSystem() {
    loadData();
    createAdmin();
}

LoginSystem::~LoginSystem() {
    writeData();
}

void LoginSystem::loadData() {
    ifstream in("data\\logins.bin", ios::binary);
    if (checkInpFile(in)) {
        int size;
        in.read((char*)&size, sizeof(size));
        for (int i = 0; i < size; ++i) {
            string login, salt;
            bReadStr(login, in);
            bReadStr(salt, in);
            logins.insert(login);
            loginToSalt[login] = salt;
        }
    }
    in.close();

    in.open("data\\passwords.bin", ios::binary);
    if (checkInpFile(in)) {
        int size;
        in.read((char*)&size, sizeof(size));
        for (int i = 0; i < size; ++i) {
            string login;
            size_t hash;
            bReadStr(login, in);
            in.read((char*)&hash, sizeof(hash));
            loginToHashedPassword[login] = hash;
        }
    }
    in.close();

    in.open("data\\roles.bin", ios::binary);
    if (checkInpFile(in)) {
        int size;
        in.read((char*)&size, sizeof(size));
        for (int i = 0; i < size; ++i) {
            string login, role;
            bReadStr(login, in);
            bReadStr(role, in);
            loginToRole[login] = role;
        }
    }
    in.close();
}

void LoginSystem::writeData() {
    std::ofstream out("data\\logins.bin", std::ios::binary);
    int size = static_cast<int>(loginToSalt.size());
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (const auto& pair : loginToSalt) {
        bWriteStr(pair.first, out);
        bWriteStr(pair.second, out);
    }
    out.close();

    out.open("data\\passwords.bin", std::ios::binary);
    size = static_cast<int>(loginToHashedPassword.size());
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (const auto& pair : loginToHashedPassword) {
        bWriteStr(pair.first, out);
        out.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
    }
    out.close();

    out.open("data\\roles.bin", std::ios::binary);
    size = static_cast<int>(loginToRole.size());
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (const auto& pair : loginToRole) {
        bWriteStr(pair.first, out);
        bWriteStr(pair.second, out);
    }
    out.close();
}

string LoginSystem::generateSalt(size_t length) {
    const string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, chars.size() - 1);
    string salt;
    for (size_t i = 0; i < length; ++i)
        salt += chars[dist(gen)];
    return salt;
}

string LoginSystem::createUserFile(size_t hash) {
    ofstream out("data\\users.bin", ios::binary | ios::app);
    out.write((char*)&hash, sizeof(hash));
    streampos pos = out.tellp();
    out.close();
    size_t fileNum = pos / sizeof(hash);
    string path = "data\\users\\" + to_string(fileNum) + ".bin";
    ofstream f(path, ios::binary);
    f.close();
    return path;
}

void LoginSystem::openUserFile(string& path, size_t hash) {
    ifstream in("data\\users.bin", ios::binary);
    if (!checkInpFile(in)) return;
    size_t tmp, n = 0;
    while (in.read((char*)&tmp, sizeof(tmp))) {
        ++n;
        if (tmp == hash) break;
    }
    in.close();
    path = "data\\users\\" + to_string(n) + ".bin";
}

void LoginSystem::rewriteUserFile(size_t oldHash, size_t newHash) {
    fstream f("data\\users.bin", ios::binary | ios::in | ios::out);
    if (!f) return;
    size_t tmp;
    while (f.read((char*)&tmp, sizeof(tmp))) {
        if (tmp == oldHash) {
            f.seekp(-static_cast<std::streamoff>(sizeof(tmp)), ios::cur);
            f.write((char*)&newHash, sizeof(newHash));
            break;
        }
    }
    f.close();
}

void LoginSystem::createAdmin() {
    if (!logins.count("admin")) {
        string salt = generateSalt(16);
        string password = "admin";

        std::hash<std::string> hasher;
        size_t hash = hasher(salt + password);

        logins.insert("admin");
        loginToSalt["admin"] = salt;
        loginToHashedPassword["admin"] = hash;
        loginToRole["admin"] = "admin";

        string path = createUserFile(hash);
        writeData();

        EmployeeUser admin(path, "admin", "admin");
    }
}


void LoginSystem::assignRole(const string& login, const string& role) {
    if (!logins.count(login)) {
        cout << "Пользователь не найден.\n";
        return;
    }
    loginToRole[login] = role;
    cout << "Роль пользователя обновлена на: " << role << "\n";
}

bool LoginSystem::authentication(const string& login, const string& password, string& path) {
    if (!logins.count(login)) return false;
    string salt = loginToSalt[login];
    std::hash<std::string> hasher;
    size_t hash = hasher(salt + password);
    if (hash == loginToHashedPassword[login]) {
        openUserFile(path, hash);
        return true;
    }
    return false;
}

shared_ptr<EmployeeUser> LoginSystem::signUp() {
    string login, password, confirm;
    get("Введите логин: ", login);
    while (logins.count(login)) get("Логин занят. Введите другой: ", login);
    getPassword("Введите пароль: ", password);
    getPassword("Подтвердите пароль: ", confirm);

    while (password != confirm) {
        getPassword("Пароли не совпадают. Повторите: ", password);
        getPassword("Подтвердите пароль: ", confirm);
    }

    string role = "user";

    string salt = generateSalt(16);
    std::hash<std::string> hasher;
    size_t hash = hasher(salt + password);

    logins.insert(login);
    loginToSalt[login] = salt;
    loginToHashedPassword[login] = hash;
    loginToRole[login] = role; 

    string path = createUserFile(hash);
    writeData();

    cout << "Регистрация завершена! Вы зарегистрированы как сотрудник.\n";
    return make_shared<EmployeeUser>(path, login, role);
}

shared_ptr<EmployeeUser> LoginSystem::logIn() {
    string login, password, path;
    get("Введите логин: ", login);
    getPassword("Введите пароль: ", password);
    if (!authentication(login, password, path)) {
        cout << "Неверный логин или пароль.\n";
        return nullptr;
    }
    string role = loginToRole[login];
    return make_shared<EmployeeUser>(path, login, role);
}

void LoginSystem::changePassword(shared_ptr<EmployeeUser>& user) {
    string login = user->getLogin();
    string password, confirm;
    getPassword("Введите новый пароль: ", password);
    getPassword("Подтвердите новый пароль: ", confirm);
    while (password != confirm) {
        getPassword("Пароли не совпадают. Повторите: ", password);
        getPassword("Подтвердите пароль: ", confirm);
    }

    string salt = generateSalt(16);
    std::hash<std::string> hasher;
    size_t newHash = hasher(salt + password);
    size_t oldHash = loginToHashedPassword[login];
    loginToSalt[login] = salt;
    loginToHashedPassword[login] = newHash;
    rewriteUserFile(oldHash, newHash);
    writeData();
    cout << "Пароль успешно изменён!\n";
}

Base* LoginSystem::run()
{
    shared_ptr<EmployeeUser> user;
    int authChoice;
    do {
        system("CLS");
        std::cout << "=====================================\n";
        std::cout << "     СИСТЕМА РАСЧЁТА ЗАРПЛАТЫ\n";
        std::cout << "=====================================\n";
        std::cout << "[1] Войти\n";
        std::cout << "[2] Зарегистрироваться\n";
        std::cout << "[3] Войти как гость\n";
        std::cout << "[0] Выход\n";
        getInRange("Ваш выбор: ", authChoice, 0, 3);

        switch (authChoice) {
        case 1:
            user = logIn();
            break;
        case 2:
            user = signUp();
            break;
        case 3:
            return new GuestEmployeeBase(); 
        case 0:
            return nullptr;
        }
    } while (!user);


    std::cout << "Пользователь " << user->getLogin()
        << " с ролью " << user->getRole() << " успешно аутентифицирован\n";
    system("pause");

    int choice = 0;
    while (true) {
        system("CLS");
        std::string role = user->getRole();

        std::cout << "-------------------------------------------------------------------------------------------\n"
            << "|                                СИСТЕМА РАСЧЁТА ЗАРПЛАТЫ                                 |\n"
            << "|        [1] Войти в интерфейс роли                                                       |\n"
            << "|        [2] Сменить пароль                                                               |\n";
        if (role == "admin")
            std::cout << "|        [3] Назначить администратора                                                     |\n";
        std::cout << "|        [0] Выход                                                                         |\n"
            << "-------------------------------------------------------------------------------------------\n";

        getInRange("Ваш выбор: ", choice, 0, role == "admin" ? 3 : 2);

        switch (choice) {
        case 1:
            if (role == "admin") {
                return new AdminEmployeeBase(user);
            }
            if (role == "user") {
                return new UserPayrollBase(user);
            }
            return new GuestEmployeeBase();

        case 2:
            changePassword(user);
            system("pause");
            break;
        case 3:
            if (role == "admin") {
                std::string targetLogin;
                get("Введите логин пользователя: ", targetLogin);
                assignRole(targetLogin, "admin");
                system("pause");
            }
            break;
        case 0:
            return nullptr;
        }
    }
}