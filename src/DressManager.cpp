#include "DressManager.h"
#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

bool DressManager::createDress(const Dress& dress) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Dresses (DressName, Category, Size, Color, RentalPrice, ConditionStatus, AvailabilityStatus) "
            "VALUES (?, ?, ?, ?, ?, ?, ?)"
        );
        
        pstmt->setString(1, dress.DressName);
        pstmt->setString(2, dress.Category);
        pstmt->setString(3, dress.Size);
        pstmt->setString(4, dress.Color);
        pstmt->setDouble(5, dress.RentalPrice);
        pstmt->setString(6, dress.ConditionStatus);
        pstmt->setString(7, dress.AvailabilityStatus);
        
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Error creating dress: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

Dress* DressManager::getDressByID(int dressID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return nullptr;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Dresses WHERE DressID = ?"
        );
        pstmt->setInt(1, dressID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            Dress* dress = new Dress();
            dress->DressID = res->getInt("DressID");
            dress->DressName = res->getString("DressName");
            dress->Category = res->getString("Category");
            dress->Size = res->getString("Size");
            dress->Color = res->getString("Color");
            dress->RentalPrice = res->getDouble("RentalPrice");
            dress->ConditionStatus = res->getString("ConditionStatus");
            dress->AvailabilityStatus = res->getString("AvailabilityStatus");
            delete pstmt;
            delete res;
            return dress;
        }
        delete pstmt;
        if (res) delete res;
        return nullptr;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting dress: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<Dress> DressManager::getAllDresses() {
    std::vector<Dress> dresses;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Dresses ORDER BY DressID"
        );
        
        while (res && res->next()) {
            Dress dress;
            dress.DressID = res->getInt("DressID");
            dress.DressName = res->getString("DressName");
            dress.Category = res->getString("Category");
            dress.Size = res->getString("Size");
            dress.Color = res->getString("Color");
            dress.RentalPrice = res->getDouble("RentalPrice");
            dress.ConditionStatus = res->getString("ConditionStatus");
            dress.AvailabilityStatus = res->getString("AvailabilityStatus");
            dresses.push_back(dress);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting dresses: " << e.what() << std::endl;
    }
    return dresses;
}

std::vector<Dress> DressManager::getAvailableDresses() {
    std::vector<Dress> dresses;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Dresses WHERE AvailabilityStatus = 'Available' ORDER BY DressID"
        );
        
        while (res && res->next()) {
            Dress dress;
            dress.DressID = res->getInt("DressID");
            dress.DressName = res->getString("DressName");
            dress.Category = res->getString("Category");
            dress.Size = res->getString("Size");
            dress.Color = res->getString("Color");
            dress.RentalPrice = res->getDouble("RentalPrice");
            dress.ConditionStatus = res->getString("ConditionStatus");
            dress.AvailabilityStatus = res->getString("AvailabilityStatus");
            dresses.push_back(dress);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting available dresses: " << e.what() << std::endl;
    }
    return dresses;
}

std::vector<Dress> DressManager::searchDresses(const std::string& searchTerm) {
    std::vector<Dress> dresses;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return dresses;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Dresses WHERE DressName LIKE ? OR Category LIKE ? OR Color LIKE ? OR Size LIKE ?"
        );
        std::string pattern = "%" + searchTerm + "%";
        pstmt->setString(1, pattern);
        pstmt->setString(2, pattern);
        pstmt->setString(3, pattern);
        pstmt->setString(4, pattern);
        
        sql::ResultSet* res = pstmt->executeQuery();
        while (res && res->next()) {
            Dress dress;
            dress.DressID = res->getInt("DressID");
            dress.DressName = res->getString("DressName");
            dress.Category = res->getString("Category");
            dress.Size = res->getString("Size");
            dress.Color = res->getString("Color");
            dress.RentalPrice = res->getDouble("RentalPrice");
            dress.ConditionStatus = res->getString("ConditionStatus");
            dress.AvailabilityStatus = res->getString("AvailabilityStatus");
            dresses.push_back(dress);
        }
        if (res) delete res;
        delete pstmt;
    } catch (sql::SQLException& e) {
        std::cerr << "Error searching dresses: " << e.what() << std::endl;
    }
    return dresses;
}

std::vector<Dress> DressManager::getDressesByCategory(const std::string& category) {
    std::vector<Dress> dresses;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return dresses;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Dresses WHERE Category = ? AND AvailabilityStatus = 'Available'"
        );
        pstmt->setString(1, category);
        sql::ResultSet* res = pstmt->executeQuery();
        
        while (res && res->next()) {
            Dress dress;
            dress.DressID = res->getInt("DressID");
            dress.DressName = res->getString("DressName");
            dress.Category = res->getString("Category");
            dress.Size = res->getString("Size");
            dress.Color = res->getString("Color");
            dress.RentalPrice = res->getDouble("RentalPrice");
            dress.ConditionStatus = res->getString("ConditionStatus");
            dress.AvailabilityStatus = res->getString("AvailabilityStatus");
            dresses.push_back(dress);
        }
        if (res) delete res;
        delete pstmt;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting dresses by category: " << e.what() << std::endl;
    }
    return dresses;
}

bool DressManager::updateDress(int dressID, const Dress& dress) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE Dresses SET DressName = ?, Category = ?, Size = ?, Color = ?, "
            "RentalPrice = ?, ConditionStatus = ?, AvailabilityStatus = ? WHERE DressID = ?"
        );
        
        pstmt->setString(1, dress.DressName);
        pstmt->setString(2, dress.Category);
        pstmt->setString(3, dress.Size);
        pstmt->setString(4, dress.Color);
        pstmt->setDouble(5, dress.RentalPrice);
        pstmt->setString(6, dress.ConditionStatus);
        pstmt->setString(7, dress.AvailabilityStatus);
        pstmt->setInt(8, dressID);
        
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error updating dress: " << e.what() << std::endl;
        return false;
    }
}

bool DressManager::deleteDress(int dressID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "DELETE FROM Dresses WHERE DressID = ?"
        );
        pstmt->setInt(1, dressID);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error deleting dress: " << e.what() << std::endl;
        return false;
    }
}

bool DressManager::updateAvailability(int dressID, const std::string& status) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE Dresses SET AvailabilityStatus = ? WHERE DressID = ?"
        );
        pstmt->setString(1, status);
        pstmt->setInt(2, dressID);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error updating availability: " << e.what() << std::endl;
        return false;
    }
}

bool DressManager::isDressAvailable(int dressID, const std::string& startDate, const std::string& endDate) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT COUNT(*) as count FROM RentalItems ri "
            "JOIN Rentals r ON ri.RentalID = r.RentalID "
            "WHERE ri.DressID = ? AND r.Status = 'Active' "
            "AND ((r.RentalDate <= ? AND r.DueDate >= ?) OR (r.RentalDate <= ? AND r.DueDate >= ?))"
        );
        pstmt->setInt(1, dressID);
        pstmt->setString(2, endDate);
        pstmt->setString(3, startDate);
        pstmt->setString(4, endDate);
        pstmt->setString(5, startDate);
        
        sql::ResultSet* res = pstmt->executeQuery();
        bool available = true;
        if (res && res->next()) {
            available = res->getInt("count") == 0;
        }
        delete pstmt;
        if (res) delete res;
        return available;
    } catch (sql::SQLException& e) {
        std::cerr << "Error checking availability: " << e.what() << std::endl;
        return false;
    }
}

void DressManager::displayDress(const Dress& dress) {
    std::cout << std::endl;
    UIColors::printSeparator(60);
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Dress ID:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.DressID 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Name:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.DressName 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Category:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.Category 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Size:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.Size 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Color:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.Color 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::ostringstream priceStream;
    priceStream << std::fixed << std::setprecision(2) << "RM " << dress.RentalPrice;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Rental Price:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << priceStream.str()
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Condition:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.ConditionStatus 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Availability:", UIColors::YELLOW) << " " 
              << std::setw(40) << std::left << dress.AvailabilityStatus 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    UIColors::printSeparator(60);
}

void DressManager::displayAllDresses(const std::vector<Dress>& dresses) {
    if (dresses.empty()) {
        UIColors::printInfo("No dresses found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("DRESS INVENTORY LIST", 140, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(140);
    std::cout << std::setw(8) << UIColors::colorize("ID", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(25) << UIColors::colorize("Name", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Category", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(8) << UIColors::colorize("Size", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Color", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(12) << UIColors::colorize("Price", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Condition", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Availability", UIColors::BOLD + UIColors::YELLOW) << std::endl;
    UIColors::printSeparator(140);
    
    for (const auto& dress : dresses) {
        std::cout << std::setw(8) << dress.DressID
                  << std::setw(25) << dress.DressName.substr(0, 23)
                  << std::setw(15) << dress.Category
                  << std::setw(8) << dress.Size
                  << std::setw(15) << dress.Color
                  << std::setw(12) << std::fixed << std::setprecision(2) << dress.RentalPrice
                  << std::setw(15) << dress.ConditionStatus
                  << std::setw(15) << dress.AvailabilityStatus << std::endl;
    }
    UIColors::printSeparator(140);
}

double DressManager::getDressPrice(int dressID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return 0.0;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT RentalPrice FROM Dresses WHERE DressID = ?"
        );
        pstmt->setInt(1, dressID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        double price = 0.0;
        if (res && res->next()) {
            price = res->getDouble("RentalPrice");
        }
        delete pstmt;
        if (res) delete res;
        return price;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting dress price: " << e.what() << std::endl;
        return 0.0;
    }
}

