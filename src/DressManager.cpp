#include "DressManager.h"
#include "UIColors.h"
#include "MenuHandlers.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Helper function to safely get CleaningStatus from result set
static std::string safeGetCleaningStatus(sql::ResultSet* res) {
    try {
        std::string status = res->getString("CleaningStatus");
        return status.empty() ? "Clean" : status;
    } catch (sql::SQLException&) {
        // Column doesn't exist, return default
        return "Clean";
    }
}

bool DressManager::createDress(const Dress& dress) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Dresses (DressName, Category, Size, Color, RentalPrice, ConditionStatus, AvailabilityStatus, CleaningStatus) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"
        );
        
        pstmt->setString(1, dress.DressName);
        pstmt->setString(2, dress.Category);
        pstmt->setString(3, dress.Size);
        pstmt->setString(4, dress.Color);
        pstmt->setDouble(5, dress.RentalPrice);
        pstmt->setString(6, dress.ConditionStatus);
        pstmt->setString(7, dress.AvailabilityStatus);
        pstmt->setString(8, dress.CleaningStatus.empty() ? "Clean" : dress.CleaningStatus);
        
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
            dress->CleaningStatus = safeGetCleaningStatus(res);
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
            dress.CleaningStatus = safeGetCleaningStatus(res);
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
            dress.CleaningStatus = safeGetCleaningStatus(res);
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
            dress.CleaningStatus = safeGetCleaningStatus(res);
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
            dress.CleaningStatus = safeGetCleaningStatus(res);
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
            "RentalPrice = ?, ConditionStatus = ?, AvailabilityStatus = ?, CleaningStatus = ? WHERE DressID = ?"
        );
        
        pstmt->setString(1, dress.DressName);
        pstmt->setString(2, dress.Category);
        pstmt->setString(3, dress.Size);
        pstmt->setString(4, dress.Color);
        pstmt->setDouble(5, dress.RentalPrice);
        pstmt->setString(6, dress.ConditionStatus);
        pstmt->setString(7, dress.AvailabilityStatus);
        pstmt->setString(8, dress.CleaningStatus.empty() ? "Clean" : dress.CleaningStatus);
        pstmt->setInt(9, dressID);
        
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
              << UIColors::colorize("Dress ID:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.DressID 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Name:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.DressName 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Category:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.Category 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Size:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.Size 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Color:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.Color 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::ostringstream priceStream;
    priceStream << std::fixed << std::setprecision(2) << "RM " << dress.RentalPrice;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Rental Price:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << priceStream.str()
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Condition:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.ConditionStatus 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Availability:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.AvailabilityStatus 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    std::cout << UIColors::colorize("|", UIColors::CYAN) << " " 
              << UIColors::colorize("Cleaning Status:", UIColors::CYAN) << " " 
              << std::setw(40) << std::left << dress.CleaningStatus 
              << UIColors::colorize("|", UIColors::CYAN) << std::endl;
    UIColors::printSeparator(60);
}

void DressManager::displayAllDresses(const std::vector<Dress>& dresses) {
    if (dresses.empty()) {
        UIColors::printInfo("No dresses found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("DRESS INVENTORY LIST", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(SCREEN_WIDTH);
    
    // Calculate column widths to fit within SCREEN_WIDTH with proper spacing and borders
    int col1 = 5, col2 = 18, col3 = 14, col4 = 7, col5 = 10, col6 = 12, col7 = 12, col8 = 13, col9 = 14;
    int totalWidth = col1 + col2 + col3 + col4 + col5 + col6 + col7 + col8 + col9 + 10; // +10 for 9 separators + 1
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
    
    // Update border line for new width
    borderLine = std::string(padding, ' ') + "+" + std::string(totalWidth - 2, '-') + "+";
    
    // Print header with proper spacing and borders
    std::cout << std::string(padding, ' ') << "|"
              << padColored(UIColors::colorize("ID", UIColors::BOLD + UIColors::CYAN), col1) << "|"
              << padColored(UIColors::colorize("Name", UIColors::BOLD + UIColors::CYAN), col2) << "|"
              << padColored(UIColors::colorize("Category", UIColors::BOLD + UIColors::CYAN), col3) << "|"
              << padColored(UIColors::colorize("Size", UIColors::BOLD + UIColors::CYAN), col4) << "|"
              << padColored(UIColors::colorize("Color", UIColors::BOLD + UIColors::CYAN), col5) << "|"
              << padColored(UIColors::colorize("Price", UIColors::BOLD + UIColors::CYAN), col6) << "|"
              << padColored(UIColors::colorize("Condition", UIColors::BOLD + UIColors::CYAN), col7) << "|"
              << padColored(UIColors::colorize("Status", UIColors::BOLD + UIColors::CYAN), col8) << "|"
              << padColored(UIColors::colorize("Cleaning", UIColors::BOLD + UIColors::CYAN), col9) << "|"
              << std::endl;
    
    // Print separator after header
    std::cout << borderLine << std::endl;
    
    // Print data rows with proper spacing and borders
    for (const auto& dress : dresses) {
        std::ostringstream priceStream;
        priceStream << std::fixed << std::setprecision(2) << "RM " << dress.RentalPrice;
        
        std::cout << std::string(padding, ' ') << "|"
                  << std::setw(col1) << std::left << dress.DressID
                  << "|" << std::setw(col2) << std::left << (dress.DressName.length() > col2 - 1 ? dress.DressName.substr(0, col2 - 1) : dress.DressName)
                  << "|" << std::setw(col3) << std::left << (dress.Category.length() > col3 - 1 ? dress.Category.substr(0, col3 - 1) : dress.Category)
                  << "|" << std::setw(col4) << std::left << dress.Size
                  << "|" << std::setw(col5) << std::left << (dress.Color.length() > col5 - 1 ? dress.Color.substr(0, col5 - 1) : dress.Color)
                  << "|" << std::setw(col6) << std::left << priceStream.str()
                  << "|" << std::setw(col7) << std::left << (dress.ConditionStatus.length() > col7 - 1 ? dress.ConditionStatus.substr(0, col7 - 1) : dress.ConditionStatus)
                  << "|" << std::setw(col8) << std::left << (dress.AvailabilityStatus.length() > col8 - 1 ? dress.AvailabilityStatus.substr(0, col8 - 1) : dress.AvailabilityStatus)
                  << "|" << std::setw(col9) << std::left << (dress.CleaningStatus.length() > col9 - 1 ? dress.CleaningStatus.substr(0, col9 - 1) : dress.CleaningStatus)
                  << "|" << std::endl;
    }
    
    // Print bottom border
    std::cout << borderLine << std::endl;
    UIColors::printSeparator(SCREEN_WIDTH);
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

