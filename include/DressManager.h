#ifndef DRESSMANAGER_H
#define DRESSMANAGER_H

#include "DatabaseManager.h"
#include <string>
#include <vector>

struct Dress {
    int DressID;
    std::string DressName;
    std::string Category;
    std::string Size;
    std::string Color;
    double RentalPrice;
    std::string ConditionStatus;
    std::string AvailabilityStatus;
};

class DressManager {
public:
    bool createDress(const Dress& dress);
    Dress* getDressByID(int dressID);
    std::vector<Dress> getAllDresses();
    std::vector<Dress> getAvailableDresses();
    std::vector<Dress> searchDresses(const std::string& searchTerm);
    std::vector<Dress> getDressesByCategory(const std::string& category);
    bool updateDress(int dressID, const Dress& dress);
    bool deleteDress(int dressID);
    bool updateAvailability(int dressID, const std::string& status);
    bool isDressAvailable(int dressID, const std::string& startDate, const std::string& endDate);
    void displayDress(const Dress& dress);
    void displayAllDresses(const std::vector<Dress>& dresses);
    double getDressPrice(int dressID);
};

#endif

