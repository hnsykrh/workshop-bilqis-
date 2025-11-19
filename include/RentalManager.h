#ifndef RENTALMANAGER_H
#define RENTALMANAGER_H

#include "DatabaseManager.h"
#include "CustomerManager.h"
#include "DressManager.h"
#include <string>
#include <vector>
#include <ctime>

struct Rental {
    int RentalID;
    int CustomerID;
    std::string RentalDate;
    std::string DueDate;
    std::string ReturnDate;
    double TotalAmount;
    double LateFee;
    std::string Status;
};

struct RentalItem {
    int RentalItemID;
    int RentalID;
    int DressID;
    double RentalPrice;
};

class RentalManager {
public:
    int createRental(int customerID, const std::string& rentalDate, int duration, const std::vector<int>& dressIDs);
    Rental* getRentalByID(int rentalID);
    std::vector<Rental> getRentalsByCustomer(int customerID);
    std::vector<Rental> getActiveRentals();
    std::vector<Rental> getOverdueRentals();
    std::vector<RentalItem> getRentalItems(int rentalID);
    bool returnRental(int rentalID, const std::string& returnDate);
    bool calculateLateFee(int rentalID);
    bool canRentDress(int dressID, const std::string& startDate, const std::string& endDate);
    std::string calculateDueDate(const std::string& rentalDate, int duration);
    void displayRental(const Rental& rental);
    void displayRentalDetails(int rentalID);
    bool validateRentalDuration(int duration);
    bool validateMaxRentals(int customerID);
    bool validateDressStatus(int dressID);
};

#endif

