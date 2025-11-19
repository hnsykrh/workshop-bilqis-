#include "CustomerManager.h"
#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

bool CustomerManager::createCustomer(const Customer& customer) {
    if (!validateAge(customer.DateOfBirth)) {
        std::cerr << "Error: Customer must be at least 18 years old." << std::endl;
        return false;
    }
    
    if (isICExists(customer.IC_Number)) {
        std::cerr << "Error: IC Number already exists." << std::endl;
        return false;
    }
    
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Customers (Name, IC_Number, Phone, Email, Address, DateOfBirth) "
            "VALUES (?, ?, ?, ?, ?, ?)"
        );
        
        pstmt->setString(1, customer.Name);
        pstmt->setString(2, customer.IC_Number);
        pstmt->setString(3, customer.Phone);
        pstmt->setString(4, customer.Email);
        pstmt->setString(5, customer.Address);
        pstmt->setString(6, customer.DateOfBirth);
        
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Error creating customer: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

Customer* CustomerManager::getCustomerByID(int customerID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return nullptr;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Customers WHERE CustomerID = ?"
        );
        pstmt->setInt(1, customerID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            Customer* customer = new Customer();
            customer->CustomerID = res->getInt("CustomerID");
            customer->Name = res->getString("Name");
            customer->IC_Number = res->getString("IC_Number");
            customer->Phone = res->getString("Phone");
            customer->Email = res->getString("Email");
            customer->Address = res->getString("Address");
            customer->DateOfBirth = res->getString("DateOfBirth");
            delete pstmt;
            delete res;
            return customer;
        }
        delete pstmt;
        delete res;
        return nullptr;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting customer: " << e.what() << std::endl;
        return nullptr;
    }
}

Customer* CustomerManager::getCustomerByIC(const std::string& icNumber) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return nullptr;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Customers WHERE IC_Number = ?"
        );
        pstmt->setString(1, icNumber);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            Customer* customer = new Customer();
            customer->CustomerID = res->getInt("CustomerID");
            customer->Name = res->getString("Name");
            customer->IC_Number = res->getString("IC_Number");
            customer->Phone = res->getString("Phone");
            customer->Email = res->getString("Email");
            customer->Address = res->getString("Address");
            customer->DateOfBirth = res->getString("DateOfBirth");
            delete pstmt;
            delete res;
            return customer;
        }
        delete pstmt;
        delete res;
        return nullptr;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting customer: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<Customer> CustomerManager::getAllCustomers() {
    std::vector<Customer> customers;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Customers ORDER BY CustomerID"
        );
        
        while (res && res->next()) {
            Customer customer;
            customer.CustomerID = res->getInt("CustomerID");
            customer.Name = res->getString("Name");
            customer.IC_Number = res->getString("IC_Number");
            customer.Phone = res->getString("Phone");
            customer.Email = res->getString("Email");
            customer.Address = res->getString("Address");
            customer.DateOfBirth = res->getString("DateOfBirth");
            customers.push_back(customer);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting customers: " << e.what() << std::endl;
    }
    return customers;
}

std::vector<Customer> CustomerManager::searchCustomers(const std::string& searchTerm) {
    std::vector<Customer> customers;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return customers;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Customers WHERE Name LIKE ? OR IC_Number LIKE ? OR Phone LIKE ? OR Email LIKE ?"
        );
        std::string pattern = "%" + searchTerm + "%";
        pstmt->setString(1, pattern);
        pstmt->setString(2, pattern);
        pstmt->setString(3, pattern);
        pstmt->setString(4, pattern);
        
        sql::ResultSet* res = pstmt->executeQuery();
        while (res && res->next()) {
            Customer customer;
            customer.CustomerID = res->getInt("CustomerID");
            customer.Name = res->getString("Name");
            customer.IC_Number = res->getString("IC_Number");
            customer.Phone = res->getString("Phone");
            customer.Email = res->getString("Email");
            customer.Address = res->getString("Address");
            customer.DateOfBirth = res->getString("DateOfBirth");
            customers.push_back(customer);
        }
        if (res) delete res;
        delete pstmt;
    } catch (sql::SQLException& e) {
        std::cerr << "Error searching customers: " << e.what() << std::endl;
    }
    return customers;
}

bool CustomerManager::updateCustomer(int customerID, const Customer& customer) {
    if (!validateAge(customer.DateOfBirth)) {
        std::cerr << "Error: Customer must be at least 18 years old." << std::endl;
        return false;
    }
    
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE Customers SET Name = ?, IC_Number = ?, Phone = ?, Email = ?, Address = ?, DateOfBirth = ? "
            "WHERE CustomerID = ?"
        );
        
        pstmt->setString(1, customer.Name);
        pstmt->setString(2, customer.IC_Number);
        pstmt->setString(3, customer.Phone);
        pstmt->setString(4, customer.Email);
        pstmt->setString(5, customer.Address);
        pstmt->setString(6, customer.DateOfBirth);
        pstmt->setInt(7, customerID);
        
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error updating customer: " << e.what() << std::endl;
        return false;
    }
}

bool CustomerManager::deleteCustomer(int customerID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "DELETE FROM Customers WHERE CustomerID = ?"
        );
        pstmt->setInt(1, customerID);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error deleting customer: " << e.what() << std::endl;
        return false;
    }
}

bool CustomerManager::validateAge(const std::string& dateOfBirth) {
    std::tm tm = {};
    std::istringstream ss(dateOfBirth);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    
    if (ss.fail()) return false;
    
    std::time_t birthTime = std::mktime(&tm);
    std::time_t now = std::time(nullptr);
    double diff = std::difftime(now, birthTime);
    int years = static_cast<int>(diff / (365.25 * 24 * 60 * 60));
    
    return years >= 18;
}

bool CustomerManager::isICExists(const std::string& icNumber) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT COUNT(*) as count FROM Customers WHERE IC_Number = ?"
        );
        pstmt->setString(1, icNumber);
        sql::ResultSet* res = pstmt->executeQuery();
        
        bool exists = false;
        if (res && res->next()) {
            exists = res->getInt("count") > 0;
        }
        delete pstmt;
        if (res) delete res;
        return exists;
    } catch (sql::SQLException& e) {
        std::cerr << "Error checking IC: " << e.what() << std::endl;
        return false;
    }
}

void CustomerManager::displayCustomer(const Customer& customer) {
    std::cout << std::endl;
    UIColors::printSeparator(60);
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Customer ID:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.CustomerID 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Name:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.Name 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("IC Number:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.IC_Number 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Phone:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.Phone 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Email:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.Email 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Address:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.Address 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Date of Birth:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << customer.DateOfBirth 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    UIColors::printSeparator(60);
}

void CustomerManager::displayAllCustomers(const std::vector<Customer>& customers) {
    if (customers.empty()) {
        UIColors::printInfo("No customers found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("CUSTOMER LIST", 120, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(120);
    std::cout << std::setw(8) << UIColors::colorize("ID", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(25) << UIColors::colorize("Name", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(18) << UIColors::colorize("IC Number", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(18) << UIColors::colorize("Phone", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(25) << UIColors::colorize("Email", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(12) << UIColors::colorize("DOB", UIColors::BOLD + UIColors::YELLOW) << std::endl;
    UIColors::printSeparator(120);
    
    for (const auto& customer : customers) {
        std::cout << std::setw(8) << customer.CustomerID
                  << std::setw(25) << customer.Name.substr(0, 23)
                  << std::setw(18) << customer.IC_Number
                  << std::setw(18) << customer.Phone
                  << std::setw(25) << customer.Email.substr(0, 23)
                  << std::setw(12) << customer.DateOfBirth << std::endl;
    }
    UIColors::printSeparator(120);
}

int CustomerManager::getActiveRentalCount(int customerID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return 0;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT COUNT(*) as count FROM Rentals WHERE CustomerID = ? AND Status = 'Active'"
        );
        pstmt->setInt(1, customerID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        int count = 0;
        if (res && res->next()) {
            count = res->getInt("count");
        }
        delete pstmt;
        if (res) delete res;
        return count;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting rental count: " << e.what() << std::endl;
        return 0;
    }
}

