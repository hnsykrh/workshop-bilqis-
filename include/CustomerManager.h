#ifndef CUSTOMERMANAGER_H
#define CUSTOMERMANAGER_H

#include "DatabaseManager.h"
#include <string>
#include <vector>
#include <ctime>

struct Customer {
    int CustomerID;
    std::string Name;
    std::string IC_Number;
    std::string Phone;
    std::string Email;
    std::string Address;
    std::string DateOfBirth;
};

class CustomerManager {
public:
    bool createCustomer(const Customer& customer);
    Customer* getCustomerByID(int customerID);
    Customer* getCustomerByIC(const std::string& icNumber);
    std::vector<Customer> getAllCustomers();
    std::vector<Customer> searchCustomers(const std::string& searchTerm);
    bool updateCustomer(int customerID, const Customer& customer);
    bool deleteCustomer(int customerID);
    bool validateAge(const std::string& dateOfBirth);
    bool isICExists(const std::string& icNumber);
    void displayCustomer(const Customer& customer);
    void displayAllCustomers(const std::vector<Customer>& customers);
    int getActiveRentalCount(int customerID);
};

#endif

