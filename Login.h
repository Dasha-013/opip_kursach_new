#pragma once
#include <set>
#include <map>
#include <string>
#include <memory>
#include <random>
#include "Base.h" 

class LoginSystem {
    std::set<std::string> logins;//логины 
    std::map<std::string, std::string> loginToSalt;//соль 
    std::map<std::string, size_t> loginToHashedPassword;//хешированные пароли 
    std::map<std::string, std::string> loginToRole;//роли(админ пользователь гость)

    void loadData();
    std::string createUserFile(size_t hash);
    void openUserFile(std::string& path, size_t hash);
    void rewriteUserFile(size_t oldHash, size_t newHash);
    void writeData();
    std::string generateSalt(size_t length);
    void createAdmin();

    std::shared_ptr<EmployeeUser> signUp();//регистрация 
    std::shared_ptr<EmployeeUser> logIn();//вход
    void changePassword(std::shared_ptr<EmployeeUser>& user);
    void assignRole(const std::string& login, const std::string& role);
    bool authentication(const std::string& login, const std::string& password, std::string& path);

public:
    LoginSystem();
    ~LoginSystem();

    Base* run();
};
