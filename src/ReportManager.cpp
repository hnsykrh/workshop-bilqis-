#include "ReportManager.h"
#include "UIColors.h"
#include "CustomerManager.h"
#include "DressManager.h"
#include "RentalManager.h"
#include "PaymentManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

void ReportManager::displayBarChart(const std::string& title, const std::vector<std::pair<std::string, double>>& data) {
    if (data.empty()) {
        std::cout << "\nNo data available for chart." << std::endl;
        return;
    }
    
    double maxValue = 0.0;
    for (const auto& pair : data) {
        if (pair.second > maxValue) maxValue = pair.second;
    }
    
    std::cout << "\n" << title << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    const int barWidth = 50;
    for (const auto& pair : data) {
        int barLength = maxValue > 0 ? static_cast<int>((pair.second / maxValue) * barWidth) : 0;
        std::cout << std::setw(20) << std::left << pair.first.substr(0, 18) << " |";
        std::cout << std::string(barLength, '#');
        std::cout << " " << std::fixed << std::setprecision(2) << pair.second << std::endl;
    }
    std::cout << std::string(80, '=') << std::endl;
}

void ReportManager::generateMonthlySalesReport(const std::string& year) {
    try {
        std::string query = 
            "SELECT DATE_FORMAT(r.RentalDate, '%Y-%m') as Month, "
            "SUM(r.TotalAmount + r.LateFee) as TotalSales, "
            "COUNT(DISTINCT r.RentalID) as RentalCount "
            "FROM Rentals r "
            "WHERE YEAR(r.RentalDate) = " + year + " "
            "GROUP BY DATE_FORMAT(r.RentalDate, '%Y-%m') "
            "ORDER BY Month";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << std::endl;
        std::string title = "MONTHLY SALES REPORT - " + year;
        UIColors::printCentered(title, 80, UIColors::BOLD + UIColors::CYAN);
        UIColors::printSeparator(80);
        std::cout << std::setw(15) << "Month" << std::setw(20) << "Total Sales (RM)"
                  << std::setw(20) << "Rental Count" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        std::vector<std::pair<std::string, double>> chartData;
        double totalYearSales = 0.0;
        int totalRentals = 0;
        
        while (res && res->next()) {
            std::string month = res->getString("Month");
            double sales = res->getDouble("TotalSales");
            int count = res->getInt("RentalCount");
            
            std::cout << std::setw(15) << month
                      << std::setw(20) << std::fixed << std::setprecision(2) << sales
                      << std::setw(20) << count << std::endl;
            
            chartData.push_back({month, sales});
            totalYearSales += sales;
            totalRentals += count;
        }
        
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::setw(15) << "TOTAL" << std::setw(20) << std::fixed << std::setprecision(2) << totalYearSales
                  << std::setw(20) << totalRentals << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        if (res) delete res;
        
        displayBarChart("\nMonthly Sales Trend", chartData);
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating monthly sales report: " << e.what() << std::endl;
    }
}

void ReportManager::generateInventoryValuationReport() {
    try {
        std::string query = 
            "SELECT Category, "
            "COUNT(*) as DressCount, "
            "SUM(RentalPrice) as TotalValue, "
            "AVG(RentalPrice) as AveragePrice "
            "FROM Dresses "
            "GROUP BY Category "
            "ORDER BY TotalValue DESC";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(100, '=') << std::endl;
        std::cout << "           INVENTORY VALUATION REPORT" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        std::cout << std::setw(20) << "Category" << std::setw(15) << "Dress Count"
                  << std::setw(20) << "Total Value (RM)" << std::setw(20) << "Average Price (RM)" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        
        std::vector<std::pair<std::string, double>> chartData;
        double grandTotal = 0.0;
        int totalDresses = 0;
        
        while (res && res->next()) {
            std::string category = res->getString("Category");
            int count = res->getInt("DressCount");
            double totalValue = res->getDouble("TotalValue");
            double avgPrice = res->getDouble("AveragePrice");
            
            std::cout << std::setw(20) << category
                      << std::setw(15) << count
                      << std::setw(20) << std::fixed << std::setprecision(2) << totalValue
                      << std::setw(20) << std::fixed << std::setprecision(2) << avgPrice << std::endl;
            
            chartData.push_back({category, totalValue});
            grandTotal += totalValue;
            totalDresses += count;
        }
        
        std::cout << std::string(100, '=') << std::endl;
        std::cout << std::setw(20) << "GRAND TOTAL" << std::setw(15) << totalDresses
                  << std::setw(20) << std::fixed << std::setprecision(2) << grandTotal << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        
        if (res) delete res;
        
        displayBarChart("\nInventory Value by Category", chartData);
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating inventory valuation report: " << e.what() << std::endl;
    }
}

void ReportManager::generateDressUtilizationReport() {
    try {
        std::string query = 
            "SELECT d.DressID, d.DressName, "
            "COUNT(ri.RentalItemID) as RentalCount, "
            "ROUND(COUNT(ri.RentalItemID) * 100.0 / (SELECT COUNT(*) FROM Dresses), 2) as UtilizationRate "
            "FROM Dresses d "
            "LEFT JOIN RentalItems ri ON d.DressID = ri.DressID "
            "LEFT JOIN Rentals r ON ri.RentalID = r.RentalID "
            "GROUP BY d.DressID, d.DressName "
            "ORDER BY RentalCount DESC, UtilizationRate DESC "
            "LIMIT 20";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(100, '=') << std::endl;
        std::cout << "           DRESS UTILIZATION REPORT (Top 20)" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        std::cout << std::setw(10) << "Dress ID" << std::setw(30) << "Dress Name"
                  << std::setw(15) << "Rental Count" << std::setw(20) << "Utilization Rate (%)" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        
        std::vector<std::pair<std::string, double>> chartData;
        
        while (res && res->next()) {
            int dressID = res->getInt("DressID");
            std::string dressName = res->getString("DressName");
            int rentalCount = res->getInt("RentalCount");
            double utilizationRate = res->getDouble("UtilizationRate");
            
            std::cout << std::setw(10) << dressID
                      << std::setw(30) << dressName.substr(0, 28)
                      << std::setw(15) << rentalCount
                      << std::setw(20) << std::fixed << std::setprecision(2) << utilizationRate << std::endl;
            
            chartData.push_back({dressName.substr(0, 15), static_cast<double>(rentalCount)});
        }
        
        std::cout << std::string(100, '=') << std::endl;
        
        if (res) delete res;
        
        displayBarChart("\nTop Dresses by Rental Count", chartData);
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating dress utilization report: " << e.what() << std::endl;
    }
}

void ReportManager::generateCustomerActivityReport() {
    try {
        std::string query = 
            "SELECT c.CustomerID, c.Name as CustomerName, "
            "COUNT(DISTINCT r.RentalID) as TotalRentals, "
            "SUM(r.TotalAmount + r.LateFee) as TotalSpent, "
            "AVG(r.TotalAmount + r.LateFee) as AverageRental "
            "FROM Customers c "
            "LEFT JOIN Rentals r ON c.CustomerID = r.CustomerID "
            "GROUP BY c.CustomerID, c.Name "
            "HAVING TotalRentals > 0 "
            "ORDER BY TotalSpent DESC "
            "LIMIT 20";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(120, '=') << std::endl;
        std::cout << "           CUSTOMER ACTIVITY REPORT (Top 20)" << std::endl;
        std::cout << std::string(120, '=') << std::endl;
        std::cout << std::setw(12) << "Customer ID" << std::setw(25) << "Customer Name"
                  << std::setw(18) << "Total Rentals" << std::setw(20) << "Total Spent (RM)"
                  << std::setw(20) << "Average Rental (RM)" << std::endl;
        std::cout << std::string(120, '=') << std::endl;
        
        while (res && res->next()) {
            int customerID = res->getInt("CustomerID");
            std::string name = res->getString("CustomerName");
            int totalRentals = res->getInt("TotalRentals");
            double totalSpent = res->getDouble("TotalSpent");
            double avgRental = res->getDouble("AverageRental");
            
            std::cout << std::setw(12) << customerID
                      << std::setw(25) << name.substr(0, 23)
                      << std::setw(18) << totalRentals
                      << std::setw(20) << std::fixed << std::setprecision(2) << totalSpent
                      << std::setw(20) << std::fixed << std::setprecision(2) << avgRental << std::endl;
        }
        
        std::cout << std::string(120, '=') << std::endl;
        
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating customer activity report: " << e.what() << std::endl;
    }
}

void ReportManager::generateOverdueItemsReport() {
    RentalManager rm;
    std::vector<Rental> overdueRentals = rm.getOverdueRentals();
    
    if (overdueRentals.empty()) {
        std::cout << "\nNo overdue rentals found." << std::endl;
        return;
    }
    
    std::cout << "\n" << std::string(120, '=') << std::endl;
    std::cout << "           OVERDUE ITEMS REPORT" << std::endl;
    std::cout << std::string(120, '=') << std::endl;
    std::cout << std::setw(12) << "Rental ID" << std::setw(15) << "Customer ID"
              << std::setw(15) << "Rental Date" << std::setw(15) << "Due Date"
              << std::setw(20) << "Days Overdue" << std::setw(20) << "Late Fee (RM)" << std::endl;
    std::cout << std::string(120, '=') << std::endl;
    
    CustomerManager cm;
    for (const auto& rental : overdueRentals) {
        // Calculate days overdue
        std::tm dueTm = {};
        std::istringstream ss(rental.DueDate);
        ss >> std::get_time(&dueTm, "%Y-%m-%d");
        std::time_t dueTime = std::mktime(&dueTm);
        std::time_t now = std::time(nullptr);
        int daysOverdue = static_cast<int>(std::difftime(now, dueTime) / (24 * 60 * 60));
        
        rm.calculateLateFee(rental.RentalID);
        Rental* updatedRental = rm.getRentalByID(rental.RentalID);
        double lateFee = updatedRental ? updatedRental->LateFee : 0.0;
        delete updatedRental;
        
        std::cout << std::setw(12) << rental.RentalID
                  << std::setw(15) << rental.CustomerID
                  << std::setw(15) << rental.RentalDate
                  << std::setw(15) << rental.DueDate
                  << std::setw(20) << daysOverdue
                  << std::setw(20) << std::fixed << std::setprecision(2) << lateFee << std::endl;
    }
    
    std::cout << std::string(120, '=') << std::endl;
}

void ReportManager::generateRentalSummaryReport() {
    try {
        std::string query = 
            "SELECT Status, COUNT(*) as Count, SUM(TotalAmount + LateFee) as TotalAmount "
            "FROM Rentals "
            "GROUP BY Status";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "           RENTAL SUMMARY REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::setw(20) << "Status" << std::setw(20) << "Count"
                  << std::setw(20) << "Total Amount (RM)" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        while (res && res->next()) {
            std::string status = res->getString("Status");
            int count = res->getInt("Count");
            double total = res->getDouble("TotalAmount");
            
            std::cout << std::setw(20) << status
                      << std::setw(20) << count
                      << std::setw(20) << std::fixed << std::setprecision(2) << total << std::endl;
        }
        
        std::cout << std::string(80, '=') << std::endl;
        
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating rental summary: " << e.what() << std::endl;
    }
}

void ReportManager::generateIncomeStatement(const std::string& startDate, const std::string& endDate) {
    try {
        std::string query = 
            "SELECT SUM(p.Amount) as TotalRevenue "
            "FROM Payments p "
            "JOIN Rentals r ON p.RentalID = r.RentalID "
            "WHERE p.Status = 'Completed' AND p.PaymentDate BETWEEN ? AND ?";
        
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        sql::PreparedStatement* pstmt = conn->prepareStatement(query);
        pstmt->setString(1, startDate);
        pstmt->setString(2, endDate);
        sql::ResultSet* res = pstmt->executeQuery();
        
        double totalRevenue = 0.0;
        if (res && res->next()) {
            totalRevenue = res->getDouble("TotalRevenue");
        }
        delete pstmt;
        if (res) delete res;
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "           INCOME STATEMENT" << std::endl;
        std::cout << "           Period: " << startDate << " to " << endDate << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::setw(40) << "Total Revenue:" << std::setw(20) << std::right 
                  << std::fixed << std::setprecision(2) << totalRevenue << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating income statement: " << e.what() << std::endl;
    }
}

void ReportManager::generateCustomerLoyaltyReport() {
    try {
        std::string query = 
            "SELECT c.CustomerID, c.Name, "
            "COUNT(DISTINCT r.RentalID) as RentalCount, "
            "SUM(r.TotalAmount + r.LateFee) as TotalSpent, "
            "DATEDIFF(CURDATE(), MIN(r.RentalDate)) as DaysSinceFirstRental "
            "FROM Customers c "
            "JOIN Rentals r ON c.CustomerID = r.CustomerID "
            "GROUP BY c.CustomerID, c.Name "
            "HAVING RentalCount >= 3 "
            "ORDER BY TotalSpent DESC";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(120, '=') << std::endl;
        std::cout << "           CUSTOMER LOYALTY REPORT (3+ Rentals)" << std::endl;
        std::cout << std::string(120, '=') << std::endl;
        std::cout << std::setw(12) << "Customer ID" << std::setw(25) << "Name"
                  << std::setw(18) << "Rental Count" << std::setw(20) << "Total Spent (RM)"
                  << std::setw(25) << "Days Since First Rental" << std::endl;
        std::cout << std::string(120, '=') << std::endl;
        
        while (res && res->next()) {
            int customerID = res->getInt("CustomerID");
            std::string name = res->getString("Name");
            int rentalCount = res->getInt("RentalCount");
            double totalSpent = res->getDouble("TotalSpent");
            int days = res->getInt("DaysSinceFirstRental");
            
            std::cout << std::setw(12) << customerID
                      << std::setw(25) << name.substr(0, 23)
                      << std::setw(18) << rentalCount
                      << std::setw(20) << std::fixed << std::setprecision(2) << totalSpent
                      << std::setw(25) << days << std::endl;
        }
        
        std::cout << std::string(120, '=') << std::endl;
        
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating customer loyalty report: " << e.what() << std::endl;
    }
}

void ReportManager::generateProfitMarginReport() {
    try {
        std::string query = 
            "SELECT d.Category, "
            "SUM(ri.RentalPrice) as TotalRevenue, "
            "COUNT(DISTINCT ri.DressID) as DressesRented, "
            "SUM(ri.RentalPrice) / COUNT(DISTINCT ri.DressID) as AvgRevenuePerDress "
            "FROM RentalItems ri "
            "JOIN Dresses d ON ri.DressID = d.DressID "
            "JOIN Rentals r ON ri.RentalID = r.RentalID "
            "WHERE r.Status = 'Returned' "
            "GROUP BY d.Category "
            "ORDER BY TotalRevenue DESC";
        
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(query);
        
        std::cout << "\n" << std::string(100, '=') << std::endl;
        std::cout << "           PROFIT MARGIN REPORT BY CATEGORY" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        std::cout << std::setw(20) << "Category" << std::setw(20) << "Total Revenue (RM)"
                  << std::setw(20) << "Dresses Rented" << std::setw(20) << "Avg Revenue/Dress" << std::endl;
        std::cout << std::string(100, '=') << std::endl;
        
        while (res && res->next()) {
            std::string category = res->getString("Category");
            double totalRevenue = res->getDouble("TotalRevenue");
            int dressesRented = res->getInt("DressesRented");
            double avgRevenue = res->getDouble("AvgRevenuePerDress");
            
            std::cout << std::setw(20) << category
                      << std::setw(20) << std::fixed << std::setprecision(2) << totalRevenue
                      << std::setw(20) << dressesRented
                      << std::setw(20) << std::fixed << std::setprecision(2) << avgRevenue << std::endl;
        }
        
        std::cout << std::string(100, '=') << std::endl;
        
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error generating profit margin report: " << e.what() << std::endl;
    }
}

void ReportManager::displayDashboard() {
    try {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "           SYSTEM DASHBOARD" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        // Total Customers
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect("SELECT COUNT(*) as count FROM Customers");
        int totalCustomers = 0;
        if (res && res->next()) totalCustomers = res->getInt("count");
        if (res) delete res;
        
        // Total Dresses
        res = DatabaseManager::getInstance().executeSelect("SELECT COUNT(*) as count FROM Dresses");
        int totalDresses = 0;
        if (res && res->next()) totalDresses = res->getInt("count");
        if (res) delete res;
        
        // Active Rentals
        res = DatabaseManager::getInstance().executeSelect("SELECT COUNT(*) as count FROM Rentals WHERE Status = 'Active'");
        int activeRentals = 0;
        if (res && res->next()) activeRentals = res->getInt("count");
        if (res) delete res;
        
        // Overdue Rentals
        res = DatabaseManager::getInstance().executeSelect("SELECT COUNT(*) as count FROM Rentals WHERE Status = 'Active' AND DueDate < CURDATE()");
        int overdueRentals = 0;
        if (res && res->next()) overdueRentals = res->getInt("count");
        if (res) delete res;
        
        // Monthly Revenue
        res = DatabaseManager::getInstance().executeSelect(
            "SELECT SUM(p.Amount) as total FROM Payments p "
            "WHERE p.Status = 'Completed' AND MONTH(p.PaymentDate) = MONTH(CURDATE()) "
            "AND YEAR(p.PaymentDate) = YEAR(CURDATE())"
        );
        double monthlyRevenue = 0.0;
        if (res && res->next()) monthlyRevenue = res->getDouble("total");
        if (res) delete res;
        
        std::cout << std::setw(40) << std::left << "Total Customers:" << std::setw(20) << std::right << totalCustomers << std::endl;
        std::cout << std::setw(40) << std::left << "Total Dresses:" << std::setw(20) << std::right << totalDresses << std::endl;
        std::cout << std::setw(40) << std::left << "Active Rentals:" << std::setw(20) << std::right << activeRentals << std::endl;
        std::cout << std::setw(40) << std::left << "Overdue Rentals:" << std::setw(20) << std::right << overdueRentals << std::endl;
        std::cout << std::setw(40) << std::left << "Monthly Revenue (RM):" << std::setw(20) << std::right 
                  << std::fixed << std::setprecision(2) << monthlyRevenue << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    } catch (sql::SQLException& e) {
        std::cerr << "Error displaying dashboard: " << e.what() << std::endl;
    }
}

