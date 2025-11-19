#include "AuthManager.h"
#include "UIColors.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

User* AuthManager::currentUser = nullptr;
bool AuthManager::loggedIn = false;

std::string AuthManager::hashPassword(const std::string& password) {
    // Simple SHA-256 implementation using std::hash (for workshop purposes)
    // In production, use proper crypto library like OpenSSL
    std::hash<std::string> hasher;
    size_t hash1 = hasher(password);
    size_t hash2 = hasher(password + "salt2025");
    size_t hash3 = hasher("dress_rental" + password);
    
    std::stringstream ss;
    ss << std::hex << hash1 << hash2 << hash3;
    std::string result = ss.str();
    
    // Ensure 64 characters (SHA-256 length)
    while (result.length() < 64) {
        result += std::to_string(hasher(result));
    }
    result = result.substr(0, 64);
    
    return result;
}

bool AuthManager::verifyPassword(const std::string& password, const std::string& hash) {
    std::string computedHash = hashPassword(password);
    return computedHash == hash;
}

bool AuthManager::login(const std::string& username, const std::string& password) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) return false;
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Users WHERE Username = ? AND IsActive = TRUE"
        );
        pstmt->setString(1, username);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            std::string storedHash = res->getString("PasswordHash");
            
            if (verifyPassword(password, storedHash)) {
                currentUser = new User();
                currentUser->UserID = res->getInt("UserID");
                currentUser->Username = res->getString("Username");
                currentUser->PasswordHash = storedHash;
                currentUser->Role = res->getString("Role");
                currentUser->FullName = res->getString("FullName");
                currentUser->Email = res->getString("Email");
                currentUser->Phone = res->getString("Phone");
                currentUser->IsActive = res->getBoolean("IsActive");
                currentUser->LastLogin = res->getString("LastLogin");
                
                // Update last login
                sql::PreparedStatement* updateStmt = conn->prepareStatement(
                    "UPDATE Users SET LastLogin = NOW() WHERE UserID = ?"
                );
                updateStmt->setInt(1, currentUser->UserID);
                updateStmt->executeUpdate();
                delete updateStmt;
                
                loggedIn = true;
                logActivity("User Login", "Users", currentUser->UserID, "Successful login");
                delete pstmt;
                delete res;
                return true;
            }
        }
        
        delete pstmt;
        if (res) delete res;
        return false;
    } catch (sql::SQLException& e) {
        std::cerr << "Login error: " << e.what() << std::endl;
        return false;
    }
}

bool AuthManager::logout() {
    if (currentUser) {
        logActivity("User Logout", "Users", currentUser->UserID, "User logged out");
        delete currentUser;
        currentUser = nullptr;
    }
    loggedIn = false;
    return true;
}

bool AuthManager::changePassword(int userID, const std::string& oldPassword, const std::string& newPassword) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) return false;
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT PasswordHash FROM Users WHERE UserID = ?"
        );
        pstmt->setInt(1, userID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            std::string storedHash = res->getString("PasswordHash");
            
            if (!verifyPassword(oldPassword, storedHash)) {
                delete pstmt;
                delete res;
                return false;
            }
            
            std::string newHash = hashPassword(newPassword);
            
            sql::PreparedStatement* updateStmt = conn->prepareStatement(
                "UPDATE Users SET PasswordHash = ? WHERE UserID = ?"
            );
            updateStmt->setString(1, newHash);
            updateStmt->setInt(2, userID);
            updateStmt->executeUpdate();
            delete updateStmt;
            
            logActivity("Password Change", "Users", userID, "Password changed successfully");
            delete pstmt;
            delete res;
            return true;
        }
        
        delete pstmt;
        if (res) delete res;
        return false;
    } catch (sql::SQLException& e) {
        std::cerr << "Change password error: " << e.what() << std::endl;
        return false;
    }
}

bool AuthManager::createUser(const User& user) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) return false;
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Users (Username, PasswordHash, Role, FullName, Email, Phone) "
            "VALUES (?, ?, ?, ?, ?, ?)"
        );
        
        std::string hashedPassword = hashPassword(user.PasswordHash); // PasswordHash field contains plain password
        
        pstmt->setString(1, user.Username);
        pstmt->setString(2, hashedPassword);
        pstmt->setString(3, user.Role);
        pstmt->setString(4, user.FullName);
        pstmt->setString(5, user.Email);
        pstmt->setString(6, user.Phone);
        
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Create user error: " << e.what() << std::endl;
        return false;
    }
}

User* AuthManager::getCurrentUser() {
    return currentUser;
}

bool AuthManager::isLoggedIn() {
    return loggedIn && currentUser != nullptr;
}

bool AuthManager::hasPermission(const std::string& requiredRole) {
    if (!isLoggedIn()) return false;
    
    if (requiredRole == "Administrator") {
        return currentUser->Role == "Administrator";
    } else if (requiredRole == "Staff") {
        return currentUser->Role == "Administrator" || currentUser->Role == "Staff";
    } else if (requiredRole == "Customer") {
        return currentUser->Role == "Customer" || currentUser->Role == "Staff" || currentUser->Role == "Administrator";
    }
    
    return false;
}

void AuthManager::logActivity(const std::string& action, const std::string& tableName, int recordID, const std::string& details) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) return;
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO ActivityLog (UserID, Action, TableName, RecordID, Details) "
            "VALUES (?, ?, ?, ?, ?)"
        );
        
        int userID = currentUser ? currentUser->UserID : 0;
        pstmt->setInt(1, userID);
        pstmt->setString(2, action);
        pstmt->setString(3, tableName);
        pstmt->setInt(4, recordID);
        pstmt->setString(5, details);
        
        pstmt->executeUpdate();
        delete pstmt;
    } catch (sql::SQLException& e) {
        // Silent fail for logging
    }
}

std::vector<std::pair<int, std::string>> AuthManager::getLowStockDresses(int threshold) {
    std::vector<std::pair<int, std::string>> lowStock;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) return lowStock;
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT DressID, DressName FROM Dresses "
            "WHERE AvailabilityStatus = 'Available' AND StockQuantity <= ?"
        );
        pstmt->setInt(1, threshold);
        sql::ResultSet* res = pstmt->executeQuery();
        
        while (res && res->next()) {
            lowStock.push_back({res->getInt("DressID"), res->getString("DressName")});
        }
        
        delete pstmt;
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting low stock: " << e.what() << std::endl;
    }
    return lowStock;
}

