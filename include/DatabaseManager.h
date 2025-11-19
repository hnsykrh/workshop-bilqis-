#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include <memory>

class DatabaseManager {
private:
    sql::mysql::MySQL_Driver* driver;
    sql::Connection* connection;
    std::string host;
    std::string user;
    std::string password;
    std::string database;
    
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

public:
    static DatabaseManager& getInstance();
    bool connect();
    void disconnect();
    sql::Connection* getConnection();
    bool executeQuery(const std::string& query);
    sql::ResultSet* executeSelect(const std::string& query);
    bool isConnected();
};

#endif

