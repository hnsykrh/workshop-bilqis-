#include "AuthManager.h"
#include "UIColors.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <cstring>
#include <vector>
#include <cstdint>

User* AuthManager::currentUser = nullptr;
bool AuthManager::loggedIn = false;

// SHA-256 implementation
static const uint32_t k[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static uint32_t rightRotate(uint32_t value, int amount) {
    return (value >> amount) | (value << (32 - amount));
}

std::string AuthManager::hashPassword(const std::string& password) {
    // Add salt for security
    std::string salted = "dress_rental_salt_2025_" + password;
    
    // Initialize hash values
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    // Pre-processing
    uint64_t ml = salted.length() * 8;
    salted += (char)0x80;
    while ((salted.length() % 64) != 56) {
        salted += (char)0x00;
    }
    
    // Append length
    for (int i = 7; i >= 0; i--) {
        salted += (char)((ml >> (i * 8)) & 0xff);
    }
    
    // Process in 512-bit chunks
    for (size_t chunk = 0; chunk < salted.length(); chunk += 64) {
        uint32_t w[64];
        
        // Copy chunk into first 16 words
        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)(unsigned char)salted[chunk + i * 4] << 24) |
                   ((uint32_t)(unsigned char)salted[chunk + i * 4 + 1] << 16) |
                   ((uint32_t)(unsigned char)salted[chunk + i * 4 + 2] << 8) |
                   ((uint32_t)(unsigned char)salted[chunk + i * 4 + 3]);
        }
        
        // Extend the first 16 words into the remaining 48 words
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rightRotate(w[i-15], 7) ^ rightRotate(w[i-15], 18) ^ (w[i-15] >> 3);
            uint32_t s1 = rightRotate(w[i-2], 17) ^ rightRotate(w[i-2], 19) ^ (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        
        // Initialize working variables
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h_val = h[7];
        
        // Main loop
        for (int i = 0; i < 64; i++) {
            uint32_t S1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h_val + S1 + ch + k[i] + w[i];
            uint32_t S0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            
            h_val = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        
        // Add the compressed chunk to the current hash value
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_val;
    }
    
    // Produce final hash value
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; i++) {
        ss << std::setw(8) << h[i];
    }
    
    return ss.str();
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
                std::string userRole = res->getString("Role");
                
                // Reject Customer role - only Admin and Staff can login
                if (userRole == "Customer") {
                    delete pstmt;
                    delete res;
                    return false;
                }
                
                currentUser = new User();
                currentUser->UserID = res->getInt("UserID");
                currentUser->Username = res->getString("Username");
                currentUser->PasswordHash = storedHash;
                currentUser->Role = userRole;
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
    } catch (sql::SQLException&) {
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

