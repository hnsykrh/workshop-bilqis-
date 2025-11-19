#include "RentalManager.h"
#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

std::string RentalManager::calculateDueDate(const std::string& rentalDate, int duration) {
    std::tm tm = {};
    std::istringstream ss(rentalDate);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    
    if (ss.fail()) return "";
    
    std::time_t time = std::mktime(&tm);
    time += duration * 24 * 60 * 60;
    std::tm* dueTm = std::localtime(&time);
    
    std::ostringstream oss;
    oss << std::put_time(dueTm, "%Y-%m-%d");
    return oss.str();
}

bool RentalManager::validateRentalDuration(int duration) {
    return duration >= 1 && duration <= 14;
}

bool RentalManager::validateMaxRentals(int customerID) {
    CustomerManager cm;
    int activeCount = cm.getActiveRentalCount(customerID);
    return activeCount < 3;
}

bool RentalManager::validateDressStatus(int dressID) {
    DressManager dm;
    Dress* dress = dm.getDressByID(dressID);
    if (!dress) return false;
    bool valid = dress->AvailabilityStatus == "Available";
    delete dress;
    return valid;
}

bool RentalManager::canRentDress(int dressID, const std::string& startDate, const std::string& endDate) {
    DressManager dm;
    return dm.isDressAvailable(dressID, startDate, endDate);
}

int RentalManager::createRental(int customerID, const std::string& rentalDate, int duration, const std::vector<int>& dressIDs) {
    // Validate business rules
    if (!validateRentalDuration(duration)) {
        std::cerr << "Error: Rental duration must be between 1 and 14 days." << std::endl;
        return -1;
    }
    
    if (!validateMaxRentals(customerID)) {
        std::cerr << "Error: Customer already has maximum 3 active rentals." << std::endl;
        return -1;
    }
    
    if (dressIDs.empty() || dressIDs.size() > 5) {
        std::cerr << "Error: Must rent between 1 and 5 dresses." << std::endl;
        return -1;
    }
    
    // Validate all dresses
    std::string dueDate = calculateDueDate(rentalDate, duration);
    if (dueDate.empty()) {
        std::cerr << "Error: Invalid rental date format. Please use YYYY-MM-DD format." << std::endl;
        return -1;
    }
    
    for (int dressID : dressIDs) {
        if (!validateDressStatus(dressID)) {
            std::cerr << "Error: Dress ID " << dressID << " is not available." << std::endl;
            return -1;
        }
        if (!canRentDress(dressID, rentalDate, dueDate)) {
            std::cerr << "Error: Dress ID " << dressID << " has overlapping bookings." << std::endl;
            return -1;
        }
    }
    
    // Calculate total amount
    DressManager dm;
    double totalAmount = 0.0;
    for (int dressID : dressIDs) {
        totalAmount += dm.getDressPrice(dressID) * duration;
    }
    
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return -1;
        }
        
        conn->setAutoCommit(false);
        
        // Create rental record
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Rentals (CustomerID, RentalDate, DueDate, TotalAmount, Status) "
            "VALUES (?, ?, ?, ?, 'Active')"
        );
        pstmt->setInt(1, customerID);
        pstmt->setString(2, rentalDate);
        pstmt->setString(3, dueDate);
        pstmt->setDouble(4, totalAmount);
        pstmt->executeUpdate();
        
        // Get the rental ID
        sql::Statement* stmt = conn->createStatement();
        sql::ResultSet* res = stmt->executeQuery("SELECT LAST_INSERT_ID() as id");
        int rentalID = -1;
        if (res && res->next()) {
            rentalID = res->getInt("id");
        }
        if (res) delete res;
        delete stmt;
        delete pstmt;
        
        // Create rental items and update dress availability
        for (int dressID : dressIDs) {
            double price = dm.getDressPrice(dressID);
            if (price <= 0.0) {
                std::cerr << "Error: Invalid price for dress ID " << dressID << std::endl;
                conn->rollback();
                conn->setAutoCommit(true);
                return -1;
            }
            
            pstmt = conn->prepareStatement(
                "INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES (?, ?, ?)"
            );
            pstmt->setInt(1, rentalID);
            pstmt->setInt(2, dressID);
            pstmt->setDouble(3, price);
            pstmt->executeUpdate();
            delete pstmt;
            
            // Update dress availability
            dm.updateAvailability(dressID, "Rented");
        }
        
        conn->commit();
        conn->setAutoCommit(true);
        return rentalID;
    } catch (sql::SQLException& e) {
        std::cerr << "Error creating rental: " << e.what() << std::endl;
        try {
            sql::Connection* conn = DatabaseManager::getInstance().getConnection();
            if (conn && !conn->isClosed()) {
                conn->rollback();
                conn->setAutoCommit(true);
            }
        } catch (...) {}
        return -1;
    }
}

Rental* RentalManager::getRentalByID(int rentalID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return nullptr;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Rentals WHERE RentalID = ?"
        );
        pstmt->setInt(1, rentalID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            Rental* rental = new Rental();
            rental->RentalID = res->getInt("RentalID");
            rental->CustomerID = res->getInt("CustomerID");
            rental->RentalDate = res->getString("RentalDate");
            rental->DueDate = res->getString("DueDate");
            rental->ReturnDate = res->getString("ReturnDate");
            rental->TotalAmount = res->getDouble("TotalAmount");
            rental->LateFee = res->getDouble("LateFee");
            rental->Status = res->getString("Status");
            delete pstmt;
            delete res;
            return rental;
        }
        delete pstmt;
        if (res) delete res;
        return nullptr;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting rental: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<Rental> RentalManager::getRentalsByCustomer(int customerID) {
    std::vector<Rental> rentals;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return rentals;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Rentals WHERE CustomerID = ? ORDER BY RentalDate DESC"
        );
        pstmt->setInt(1, customerID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        while (res && res->next()) {
            Rental rental;
            rental.RentalID = res->getInt("RentalID");
            rental.CustomerID = res->getInt("CustomerID");
            rental.RentalDate = res->getString("RentalDate");
            rental.DueDate = res->getString("DueDate");
            rental.ReturnDate = res->getString("ReturnDate");
            rental.TotalAmount = res->getDouble("TotalAmount");
            rental.LateFee = res->getDouble("LateFee");
            rental.Status = res->getString("Status");
            rentals.push_back(rental);
        }
        delete pstmt;
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting rentals: " << e.what() << std::endl;
    }
    return rentals;
}

std::vector<Rental> RentalManager::getActiveRentals() {
    std::vector<Rental> rentals;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Rentals WHERE Status = 'Active' ORDER BY DueDate"
        );
        
        while (res && res->next()) {
            Rental rental;
            rental.RentalID = res->getInt("RentalID");
            rental.CustomerID = res->getInt("CustomerID");
            rental.RentalDate = res->getString("RentalDate");
            rental.DueDate = res->getString("DueDate");
            rental.ReturnDate = res->getString("ReturnDate");
            rental.TotalAmount = res->getDouble("TotalAmount");
            rental.LateFee = res->getDouble("LateFee");
            rental.Status = res->getString("Status");
            rentals.push_back(rental);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting active rentals: " << e.what() << std::endl;
    }
    return rentals;
}

std::vector<Rental> RentalManager::getOverdueRentals() {
    std::vector<Rental> rentals;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Rentals WHERE Status = 'Active' AND DueDate < CURDATE() ORDER BY DueDate"
        );
        
        while (res && res->next()) {
            Rental rental;
            rental.RentalID = res->getInt("RentalID");
            rental.CustomerID = res->getInt("CustomerID");
            rental.RentalDate = res->getString("RentalDate");
            rental.DueDate = res->getString("DueDate");
            rental.ReturnDate = res->getString("ReturnDate");
            rental.TotalAmount = res->getDouble("TotalAmount");
            rental.LateFee = res->getDouble("LateFee");
            rental.Status = res->getString("Status");
            rentals.push_back(rental);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting overdue rentals: " << e.what() << std::endl;
    }
    return rentals;
}

std::vector<RentalItem> RentalManager::getRentalItems(int rentalID) {
    std::vector<RentalItem> items;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return items;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM RentalItems WHERE RentalID = ?"
        );
        pstmt->setInt(1, rentalID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        while (res && res->next()) {
            RentalItem item;
            item.RentalItemID = res->getInt("RentalItemID");
            item.RentalID = res->getInt("RentalID");
            item.DressID = res->getInt("DressID");
            item.RentalPrice = res->getDouble("RentalPrice");
            items.push_back(item);
        }
        delete pstmt;
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting rental items: " << e.what() << std::endl;
    }
    return items;
}

bool RentalManager::calculateLateFee(int rentalID) {
    try {
        Rental* rental = getRentalByID(rentalID);
        if (!rental || rental->Status != "Active") {
            delete rental;
            return false;
        }
        
        std::tm dueTm = {};
        std::istringstream ss(rental->DueDate);
        ss >> std::get_time(&dueTm, "%Y-%m-%d");
        
        std::time_t dueTime = std::mktime(&dueTm);
        std::time_t now = std::time(nullptr);
        
        if (now > dueTime) {
            double daysLate = std::difftime(now, dueTime) / (24 * 60 * 60);
            double lateFee = daysLate * 10.0; // RM 10 per day
            
            sql::Connection* conn = DatabaseManager::getInstance().getConnection();
            if (!conn) {
                std::cerr << "Error: Database connection failed." << std::endl;
                delete rental;
                return false;
            }
            sql::PreparedStatement* pstmt = conn->prepareStatement(
                "UPDATE Rentals SET LateFee = ? WHERE RentalID = ?"
            );
            pstmt->setDouble(1, lateFee);
            pstmt->setInt(2, rentalID);
            pstmt->executeUpdate();
            delete pstmt;
        }
        delete rental;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Error calculating late fee: " << e.what() << std::endl;
        return false;
    }
}

bool RentalManager::returnRental(int rentalID, const std::string& returnDate) {
    try {
        Rental* rental = getRentalByID(rentalID);
        if (!rental) return false;
        
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            delete rental;
            return false;
        }
        conn->setAutoCommit(false);
        
        // Calculate late fee
        calculateLateFee(rentalID);
        
        // Get rental items to update dress availability
        std::vector<RentalItem> items = getRentalItems(rentalID);
        DressManager dm;
        for (const auto& item : items) {
            dm.updateAvailability(item.DressID, "Available");
        }
        
        // Update rental status
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE Rentals SET ReturnDate = ?, Status = 'Returned' WHERE RentalID = ?"
        );
        pstmt->setString(1, returnDate);
        pstmt->setInt(2, rentalID);
        pstmt->executeUpdate();
        delete pstmt;
        
        conn->commit();
        conn->setAutoCommit(true);
        delete rental;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Error returning rental: " << e.what() << std::endl;
        try {
            sql::Connection* conn = DatabaseManager::getInstance().getConnection();
            if (conn && !conn->isClosed()) {
                conn->rollback();
                conn->setAutoCommit(true);
            }
        } catch (...) {}
        return false;
    }
}

void RentalManager::displayRental(const Rental& rental) {
    std::cout << std::endl;
    UIColors::printSeparator(60);
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Rental ID:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << rental.RentalID 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Customer ID:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << rental.CustomerID 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Rental Date:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << rental.RentalDate 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Due Date:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << rental.DueDate 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Return Date:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << (rental.ReturnDate.empty() ? "Not Returned" : rental.ReturnDate)
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::ostringstream amountStream;
    amountStream << std::fixed << std::setprecision(2) << "RM " << rental.TotalAmount;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Total Amount:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << amountStream.str()
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::ostringstream feeStream;
    feeStream << std::fixed << std::setprecision(2) << "RM " << rental.LateFee;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Late Fee:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << feeStream.str()
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Status:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << rental.Status 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    UIColors::printSeparator(60);
}

void RentalManager::displayRentalDetails(int rentalID) {
    Rental* rental = getRentalByID(rentalID);
    if (!rental) {
        std::cout << "Rental not found." << std::endl;
        return;
    }
    
    displayRental(*rental);
    
    std::vector<RentalItem> items = getRentalItems(rentalID);
    if (!items.empty()) {
        std::cout << "\nRental Items:" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::setw(8) << "Item ID" << std::setw(12) << "Dress ID" << std::setw(15) << "Rental Price" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        DressManager dm;
        for (const auto& item : items) {
            Dress* dress = dm.getDressByID(item.DressID);
            std::cout << std::setw(8) << item.RentalItemID
                      << std::setw(12) << item.DressID
                      << std::setw(15) << std::fixed << std::setprecision(2) << item.RentalPrice;
            if (dress) {
                std::cout << " - " << dress->DressName;
                delete dress;
            }
            std::cout << std::endl;
        }
        std::cout << std::string(80, '=') << std::endl;
    }
    delete rental;
}

