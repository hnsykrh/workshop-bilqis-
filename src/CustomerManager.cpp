#include "CustomerManager.h"
#include "UIColors.h"
#include "MenuHandlers.h"
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
    int tableWidth = 60;
    int padding = (SCREEN_WIDTH - tableWidth) / 2;
    if (padding < 0) padding = 0;
    
    std::string borderLine = std::string(padding, ' ') + "+" + std::string(tableWidth - 2, '-') + "+";
    std::cout << borderLine << std::endl;
    
    // Header row
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << UIColors::colorize("Field", UIColors::BOLD + UIColors::CYAN)
              << "|" << std::setw(37) << std::left << UIColors::colorize("Information", UIColors::BOLD + UIColors::CYAN)
              << "|" << std::endl;
    std::cout << borderLine << std::endl;
    
    // Data rows with proper alignment
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Customer ID:"
              << "|" << std::setw(37) << std::left << std::to_string(customer.CustomerID)
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Name:"
              << "|" << std::setw(37) << std::left << customer.Name
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "IC Number:"
              << "|" << std::setw(37) << std::left << customer.IC_Number
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Phone:"
              << "|" << std::setw(37) << std::left << customer.Phone
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Email:"
              << "|" << std::setw(37) << std::left << customer.Email
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Address:"
              << "|" << std::setw(37) << std::left << customer.Address
              << "|" << std::endl;
    std::cout << std::string(padding, ' ') << "|" 
              << std::setw(20) << std::left << "Date of Birth:"
              << "|" << std::setw(37) << std::left << customer.DateOfBirth
              << "|" << std::endl;
    std::cout << borderLine << std::endl;
}

void CustomerManager::displayAllCustomers(const std::vector<Customer>& customers) {
    if (customers.empty()) {
        UIColors::printInfo("No customers found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("CUSTOMER LIST", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(SCREEN_WIDTH);
    
    // Calculate column widths to fit within SCREEN_WIDTH
    int col1 = 6, col2 = 20, col3 = 14, col4 = 14, col5 = 20, col6 = 10;
    int totalWidth = col1 + col2 + col3 + col4 + col5 + col6;
    int padding = (SCREEN_WIDTH - totalWidth) / 2;
    if (padding < 0) padding = 0;
    
    std::cout << std::string(padding, ' ')
              << std::setw(col1) << UIColors::colorize("ID", UIColors::BOLD + UIColors::CYAN)
              << std::setw(col2) << UIColors::colorize("Name", UIColors::BOLD + UIColors::CYAN)
              << std::setw(col3) << UIColors::colorize("IC Number", UIColors::BOLD + UIColors::CYAN)
              << std::setw(col4) << UIColors::colorize("Phone", UIColors::BOLD + UIColors::CYAN)
              << std::setw(col5) << UIColors::colorize("Email", UIColors::BOLD + UIColors::CYAN)
              << std::setw(col6) << UIColors::colorize("DOB", UIColors::BOLD + UIColors::CYAN) << std::endl;
    UIColors::printSeparator(SCREEN_WIDTH);
    
    for (const auto& customer : customers) {
        std::cout << std::string(padding, ' ')
                  << std::setw(col1) << customer.CustomerID
                  << std::setw(col2) << (customer.Name.length() > 18 ? customer.Name.substr(0, 18) : customer.Name)
                  << std::setw(col3) << (customer.IC_Number.length() > 12 ? customer.IC_Number.substr(0, 12) : customer.IC_Number)
                  << std::setw(col4) << (customer.Phone.length() > 12 ? customer.Phone.substr(0, 12) : customer.Phone)
                  << std::setw(col5) << (customer.Email.length() > 18 ? customer.Email.substr(0, 18) : customer.Email)
                  << std::setw(col6) << customer.DateOfBirth << std::endl;
    }
    UIColors::printSeparator(SCREEN_WIDTH);
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

