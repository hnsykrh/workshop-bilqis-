#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "DatabaseManager.h"
#include <string>
#include <vector>
#include <ctime>

struct User {
    int UserID;
    std::string Username;
    std::string PasswordHash;
    std::string Role;
    std::string FullName;
    std::string Email;
    std::string Phone;
    bool IsActive;
    std::string LastLogin;
};

class AuthManager {
public:
    static std::string hashPassword(const std::string& password);
    static bool verifyPassword(const std::string& password, const std::string& hash);
    bool login(const std::string& username, const std::string& password);
    bool logout();
    bool changePassword(int userID, const std::string& oldPassword, const std::string& newPassword);
    bool createUser(const User& user);
    User* getCurrentUser();
    bool isLoggedIn();
    bool hasPermission(const std::string& requiredRole);
    void logActivity(const std::string& action, const std::string& tableName = "", int recordID = 0, const std::string& details = "");
    std::vector<std::pair<int, std::string>> getLowStockDresses(int threshold = 3);
    
private:
    static User* currentUser;
    static bool loggedIn;
};

#endif

