#include "CustomerManager.h"
#include "UIColors.h"
#include "MenuHandlers.h"
#include "RentalManager.h"
#include "PaymentManager.h"
#include "DressManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <set>

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
    
    // Helper to get plain text width (strip ANSI codes)
    auto plainWidth = [](const std::string& text) -> int {
        int width = 0;
        bool inEscape = false;
        for (char c : text) {
            if (c == '\033') inEscape = true;
            else if (inEscape && c == 'm') inEscape = false;
            else if (!inEscape) width++;
        }
        return width;
    };
    
    // Helper to pad colored text to exact width
    auto padColored = [=](const std::string& coloredText, int targetWidth) -> std::string {
        int actualWidth = plainWidth(coloredText);
        if (actualWidth >= targetWidth) return coloredText;
        return coloredText + std::string(targetWidth - actualWidth, ' ');
    };
    
    // Header row
    std::cout << std::string(padding, ' ') << "|" 
              << padColored(UIColors::colorize("Field", UIColors::BOLD + UIColors::CYAN), 20)
              << "|" << padColored(UIColors::colorize("Information", UIColors::BOLD + UIColors::CYAN), 37)
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
    
    // Get rental and payment information
    RentalManager rm;
    PaymentManager pm;
    DressManager dm;
    std::vector<Rental> rentals = rm.getRentalsByCustomer(customer.CustomerID);
    
    if (!rentals.empty()) {
        std::cout << std::endl;
        UIColors::printCentered("=== RENTAL INFORMATION ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
        std::cout << std::endl;
        
        int rentalTableWidth = 70;
        int rentalPadding = (SCREEN_WIDTH - rentalTableWidth) / 2;
        if (rentalPadding < 0) rentalPadding = 0;
        
        std::string rentalBorderLine = std::string(rentalPadding, ' ') + "+" + std::string(rentalTableWidth - 2, '-') + "+";
        
        // Helper to get plain text width (strip ANSI codes)
        auto plainWidth = [](const std::string& text) -> int {
            int width = 0;
            bool inEscape = false;
            for (char c : text) {
                if (c == '\033') inEscape = true;
                else if (inEscape && c == 'm') inEscape = false;
                else if (!inEscape) width++;
            }
            return width;
        };
        
        // Helper to pad colored text to exact width
        auto padColored = [=](const std::string& coloredText, int targetWidth) -> std::string {
            int actualWidth = plainWidth(coloredText);
            if (actualWidth >= targetWidth) return coloredText;
            return coloredText + std::string(targetWidth - actualWidth, ' ');
        };
        
        // Print header for rental table
        std::cout << rentalBorderLine << std::endl;
        std::cout << std::string(rentalPadding, ' ') << "|"
                  << padColored(UIColors::colorize("Rental ID", UIColors::BOLD + UIColors::CYAN), 10) << "|"
                  << padColored(UIColors::colorize("Dresses Rented", UIColors::BOLD + UIColors::CYAN), 25) << "|"
                  << padColored(UIColors::colorize("Rental Date", UIColors::BOLD + UIColors::CYAN), 12) << "|"
                  << padColored(UIColors::colorize("Due Date", UIColors::BOLD + UIColors::CYAN), 12) << "|"
                  << padColored(UIColors::colorize("Status", UIColors::BOLD + UIColors::CYAN), 8) << "|"
                  << std::endl;
        std::cout << rentalBorderLine << std::endl;
        
        // Display each rental with dress and payment info
        for (const auto& rental : rentals) {
            // Get dresses for this rental
            std::vector<int> dressIDs;
            try {
                sql::Connection* conn = DatabaseManager::getInstance().getConnection();
                if (conn) {
                    sql::PreparedStatement* pstmt = conn->prepareStatement(
                        "SELECT DressID FROM RentalItems WHERE RentalID = ?"
                    );
                    pstmt->setInt(1, rental.RentalID);
                    sql::ResultSet* res = pstmt->executeQuery();
                    
                    while (res && res->next()) {
                        dressIDs.push_back(res->getInt("DressID"));
                    }
                    delete pstmt;
                    if (res) delete res;
                }
            } catch (sql::SQLException& e) {
                // Continue if error
            }
            
            // Build dress names string
            std::string dressNames = "";
            if (!dressIDs.empty()) {
                for (size_t i = 0; i < dressIDs.size(); ++i) {
                    Dress* dress = dm.getDressByID(dressIDs[i]);
                    if (dress) {
                        if (i > 0) dressNames += ", ";
                        dressNames += dress->DressName;
                        delete dress;
                    }
                }
                if (dressNames.length() > 23) {
                    dressNames = dressNames.substr(0, 20) + "...";
                }
            } else {
                dressNames = "N/A";
            }
            
            // Get payment method for this rental
            std::vector<Payment> payments = pm.getPaymentsByRental(rental.RentalID);
            std::string paymentMethod = "N/A";
            if (!payments.empty()) {
                paymentMethod = payments[0].PaymentMethod;
                if (paymentMethod.length() > 8) {
                    paymentMethod = paymentMethod.substr(0, 8);
                }
            }
            
            std::cout << std::string(rentalPadding, ' ') << "|"
                      << std::setw(10) << std::left << rental.RentalID
                      << "|" << std::setw(25) << std::left << dressNames
                      << "|" << std::setw(12) << std::left << rental.RentalDate
                      << "|" << std::setw(12) << std::left << rental.DueDate
                      << "|" << std::setw(8) << std::left << (rental.Status.length() > 6 ? rental.Status.substr(0, 6) : rental.Status)
                      << "|" << std::endl;
        }
        std::cout << rentalBorderLine << std::endl;
        
        // Display payment methods summary
        std::cout << std::endl;
        UIColors::printCentered("=== PAYMENT METHODS USED ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
        std::cout << std::endl;
        
        std::set<std::string> paymentMethods;
        for (const auto& rental : rentals) {
            std::vector<Payment> payments = pm.getPaymentsByRental(rental.RentalID);
            for (const auto& payment : payments) {
                paymentMethods.insert(payment.PaymentMethod);
            }
        }
        
        if (!paymentMethods.empty()) {
            std::string methodsStr = "";
            for (const auto& method : paymentMethods) {
                if (!methodsStr.empty()) methodsStr += ", ";
                methodsStr += method;
            }
            UIColors::printCentered(methodsStr, SCREEN_WIDTH, UIColors::WHITE);
        } else {
            UIColors::printCentered("No payments recorded", SCREEN_WIDTH, UIColors::YELLOW);
        }
        std::cout << std::endl;
    } else {
        std::cout << std::endl;
        UIColors::printCentered("=== RENTAL INFORMATION ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
        std::cout << std::endl;
        UIColors::printCentered("No rentals found for this customer", SCREEN_WIDTH, UIColors::YELLOW);
        std::cout << std::endl;
    }
}

void CustomerManager::displayAllCustomers(const std::vector<Customer>& customers) {
    if (customers.empty()) {
        UIColors::printInfo("No customers found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("CUSTOMER LIST", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(SCREEN_WIDTH);
    
    // Calculate column widths to fit within SCREEN_WIDTH with proper spacing and borders
    int col1 = 6, col2 = 20, col3 = 14, col4 = 14, col5 = 20, col6 = 12;
    int totalWidth = col1 + col2 + col3 + col4 + col5 + col6 + 7; // +7 for 6 separators + 1
    int padding = (SCREEN_WIDTH - totalWidth) / 2;
    if (padding < 0) padding = 0;
    
    // Helper to get plain text width (strip ANSI codes)
    auto plainWidth = [](const std::string& text) -> int {
        int width = 0;
        bool inEscape = false;
        for (char c : text) {
            if (c == '\033') inEscape = true;
            else if (inEscape && c == 'm') inEscape = false;
            else if (!inEscape) width++;
        }
        return width;
    };
    
    // Helper to pad colored text to exact width
    auto padColored = [=](const std::string& coloredText, int targetWidth) -> std::string {
        int actualWidth = plainWidth(coloredText);
        if (actualWidth >= targetWidth) return coloredText;
        return coloredText + std::string(targetWidth - actualWidth, ' ');
    };
    
    // Create border line
    std::string borderLine = std::string(padding, ' ') + "+" + std::string(totalWidth - 2, '-') + "+";
    
    // Print top border
    std::cout << borderLine << std::endl;
    
    // Print header with proper spacing and borders
    std::cout << std::string(padding, ' ') << "|"
              << padColored(UIColors::colorize("ID", UIColors::BOLD + UIColors::CYAN), col1) << "|"
              << padColored(UIColors::colorize("Name", UIColors::BOLD + UIColors::CYAN), col2) << "|"
              << padColored(UIColors::colorize("IC Number", UIColors::BOLD + UIColors::CYAN), col3) << "|"
              << padColored(UIColors::colorize("Phone", UIColors::BOLD + UIColors::CYAN), col4) << "|"
              << padColored(UIColors::colorize("Email", UIColors::BOLD + UIColors::CYAN), col5) << "|"
              << padColored(UIColors::colorize("DOB", UIColors::BOLD + UIColors::CYAN), col6) << "|"
              << std::endl;
    
    // Print separator after header
    std::cout << borderLine << std::endl;
    
    // Print data rows with proper spacing and borders
    for (const auto& customer : customers) {
        std::cout << std::string(padding, ' ') << "|"
                  << std::setw(col1) << std::left << customer.CustomerID
                  << "|" << std::setw(col2) << std::left << (customer.Name.length() > col2 - 1 ? customer.Name.substr(0, col2 - 1) : customer.Name)
                  << "|" << std::setw(col3) << std::left << (customer.IC_Number.length() > col3 - 1 ? customer.IC_Number.substr(0, col3 - 1) : customer.IC_Number)
                  << "|" << std::setw(col4) << std::left << (customer.Phone.length() > col4 - 1 ? customer.Phone.substr(0, col4 - 1) : customer.Phone)
                  << "|" << std::setw(col5) << std::left << (customer.Email.length() > col5 - 1 ? customer.Email.substr(0, col5 - 1) : customer.Email)
                  << "|" << std::setw(col6) << std::left << customer.DateOfBirth
                  << "|" << std::endl;
    }
    
    // Print bottom border
    std::cout << borderLine << std::endl;
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

