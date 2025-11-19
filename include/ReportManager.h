#ifndef REPORTMANAGER_H
#define REPORTMANAGER_H

#include "DatabaseManager.h"
#include <string>
#include <vector>
#include <map>

struct MonthlySales {
    std::string Month;
    double TotalSales;
    int RentalCount;
};

struct InventoryValuation {
    std::string Category;
    int DressCount;
    double TotalValue;
    double AveragePrice;
};

struct DressUtilization {
    int DressID;
    std::string DressName;
    int RentalCount;
    double UtilizationRate;
};

struct CustomerActivity {
    int CustomerID;
    std::string CustomerName;
    int TotalRentals;
    double TotalSpent;
    double AverageRental;
};

class ReportManager {
public:
    void generateMonthlySalesReport(const std::string& year);
    void generateInventoryValuationReport();
    void generateDressUtilizationReport();
    void generateCustomerActivityReport();
    void generateOverdueItemsReport();
    void generateRentalSummaryReport();
    void generateIncomeStatement(const std::string& startDate, const std::string& endDate);
    void generateCustomerLoyaltyReport();
    void generateProfitMarginReport();
    void displayBarChart(const std::string& title, const std::vector<std::pair<std::string, double>>& data);
    void displayDashboard();
};

#endif

