#include "DatabaseManager.h"
#include <iostream>
#include <stdexcept>

DatabaseManager::DatabaseManager() 
    : driver(nullptr), connection(nullptr),
      host("tcp://127.0.0.1:3306"),
      user("root"),
      password(""),
      database("dress_rental_db") {
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        connection = driver->connect(host, user, password);
        connection->setSchema(database);
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Database Connection Error: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    }
}

void DatabaseManager::disconnect() {
    if (connection) {
        delete connection;
        connection = nullptr;
    }
}

sql::Connection* DatabaseManager::getConnection() {
    if (!connection || connection->isClosed()) {
        if (!connect()) {
            throw std::runtime_error("Database connection failed. Please check your MySQL server.");
        }
    }
    return connection;
}

bool DatabaseManager::executeQuery(const std::string& query) {
    try {
        if (!connection || connection->isClosed()) {
            if (!connect()) return false;
        }
        sql::Statement* stmt = connection->createStatement();
        stmt->execute(query);
        delete stmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Query Execution Error: " << e.what() << std::endl;
        return false;
    }
}

sql::ResultSet* DatabaseManager::executeSelect(const std::string& query) {
    try {
        if (!connection || connection->isClosed()) {
            if (!connect()) return nullptr;
        }
        sql::Statement* stmt = connection->createStatement();
        sql::ResultSet* res = stmt->executeQuery(query);
        return res;
    } catch (sql::SQLException& e) {
        std::cerr << "Select Query Error: " << e.what() << std::endl;
        return nullptr;
    }
}

bool DatabaseManager::isConnected() {
    return connection != nullptr && !connection->isClosed();
}

