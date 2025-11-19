#include "MenuHandlers.h"
#include "DatabaseManager.h"
#include "CustomerManager.h"
#include "DressManager.h"
#include "RentalManager.h"
#include "PaymentManager.h"
#include "ReportManager.h"
#include "InputValidator.h"
#include "UIColors.h"
#include "AuthManager.h"
#include <iostream>
#include <string>
#include <vector>

bool showLoginScreen() {
    AuthManager auth;
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    
    while (attempts < MAX_ATTEMPTS) {
        UIColors::printSeparator(SCREEN_WIDTH);
        UIColors::printCentered("DRESS RENTAL MANAGEMENT SYSTEM", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
        UIColors::printCentered("Please Login to Continue", SCREEN_WIDTH, UIColors::YELLOW);
        UIColors::printSeparator(SCREEN_WIDTH);
        std::cout << std::endl;
        
        if (attempts > 0) {
            UIColors::printWarning("Invalid username or password. Attempts remaining: " + std::to_string(MAX_ATTEMPTS - attempts));
            std::cout << std::endl;
        }
        
        std::string username = InputValidator::getString("Username: ", true, 3, 50);
        std::string password = InputValidator::getString("Password: ", true, 4, 100);
        
        if (auth.login(username, password)) {
            User* user = auth.getCurrentUser();
            UIColors::printSuccess("Login successful! Welcome, " + user->FullName + "!");
            UIColors::printInfo("Role: " + user->Role);
            
            // Show low stock alert
            std::vector<std::pair<int, std::string>> lowStock = auth.getLowStockDresses(3);
            if (!lowStock.empty()) {
                std::cout << std::endl;
                UIColors::printWarning("⚠️  LOW STOCK ALERT ⚠️");
                UIColors::printSeparator(SCREEN_WIDTH);
                UIColors::printCentered("The following dresses have low stock:", SCREEN_WIDTH, UIColors::YELLOW);
                for (const auto& item : lowStock) {
                    std::cout << "  • Dress ID " << item.first << ": " << item.second << std::endl;
                }
                UIColors::printSeparator(SCREEN_WIDTH);
            }
            
            InputValidator::pause("Press Enter to continue...");
            return true;
        } else {
            attempts++;
        }
    }
    
    UIColors::printError("Maximum login attempts reached. System will exit.");
    InputValidator::pause();
    return false;
}

void showChangePasswordMenu() {
    AuthManager auth;
    User* user = auth.getCurrentUser();
    
    if (!user) {
        UIColors::printError("No user logged in.");
        return;
    }
    
    UIColors::printHeader("CHANGE PASSWORD", SCREEN_WIDTH);
    
    std::string oldPassword = InputValidator::getString("Enter current password: ", true, 4, 100);
    std::string newPassword = InputValidator::getString("Enter new password: ", true, 4, 100);
    std::string confirmPassword = InputValidator::getString("Confirm new password: ", true, 4, 100);
    
    if (newPassword != confirmPassword) {
        UIColors::printError("New passwords do not match!");
        InputValidator::pause();
        return;
    }
    
    if (auth.changePassword(user->UserID, oldPassword, newPassword)) {
        UIColors::printSuccess("Password changed successfully!");
    } else {
        UIColors::printError("Failed to change password. Please check your current password.");
    }
    
    InputValidator::pause();
}

void displayMainMenu() {
    AuthManager auth;
    User* user = auth.getCurrentUser();
    std::string role = user ? user->Role : "Guest";
    
    UIColors::printSeparator(SCREEN_WIDTH);
    UIColors::printCentered("DRESS RENTAL MANAGEMENT SYSTEM", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
    UIColors::printCentered("Logged in as: " + (user ? user->FullName : "Guest") + " (" + role + ")", SCREEN_WIDTH, UIColors::GREEN);
    UIColors::printSeparator(SCREEN_WIDTH);
    std::cout << std::endl;
    
    int menuNum = 1;
    
    // Staff and Administrator options
    if (auth.hasPermission("Staff")) {
        UIColors::printMenuOption(menuNum++, "Customer Management");
        UIColors::printMenuOption(menuNum++, "Dress Inventory Management");
        UIColors::printMenuOption(menuNum++, "Rental Transaction Management");
        UIColors::printMenuOption(menuNum++, "Payment Processing");
    }
    
    // Reports and Dashboard
    if (auth.hasPermission("Staff")) {
        UIColors::printMenuOption(menuNum++, "Reports & Analytics");
        UIColors::printMenuOption(menuNum++, "Dashboard");
    }
    
    // Administrator only
    if (auth.hasPermission("Administrator")) {
        UIColors::printMenuOption(menuNum++, "User Management");
    }
    
    // Customer options
    if (auth.hasPermission("Customer")) {
        UIColors::printMenuOption(menuNum++, "View My Rentals");
        UIColors::printMenuOption(menuNum++, "View Available Dresses");
    }
    
    std::cout << std::endl;
    UIColors::printMenuOption(menuNum++, "Change Password");
    UIColors::printMenuOption(0, "Logout");
    UIColors::printSeparator(SCREEN_WIDTH);
}

void customerManagementMenu() {
    CustomerManager cm;
    int choice;
    
    while (true) {
        UIColors::printHeader("CUSTOMER MANAGEMENT", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "Add New Customer");
        UIColors::printMenuOption(2, "View All Customers");
        UIColors::printMenuOption(3, "Search Customers");
        UIColors::printMenuOption(4, "Update Customer");
        UIColors::printMenuOption(5, "Delete Customer");
        UIColors::printMenuOption(6, "View Customer Details");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 6);
        
        if (choice == 0) break;
        
        Customer customer;
        int customerID;
        std::string searchTerm;
        
        switch (choice) {
            case 1: {
                UIColors::printHeader("ADD NEW CUSTOMER", SCREEN_WIDTH);
                UIColors::printInfo("Please fill in all required fields (*).");
                
                customer.Name = InputValidator::getString("Name*: ", true, 2, 100);
                customer.IC_Number = InputValidator::getString("IC Number*: ", true, 8, 20);
                
                if (!InputValidator::isValidIC(customer.IC_Number)) {
                    InputValidator::showWarning("IC Number format may be invalid.");
                }
                
                customer.Phone = InputValidator::getString("Phone*: ", true, 8, 20);
                if (!InputValidator::isValidPhone(customer.Phone)) {
                    InputValidator::showWarning("Phone number format may be invalid.");
                }
                
                customer.Email = InputValidator::getString("Email (optional): ", false, 0, 100);
                if (!customer.Email.empty() && !InputValidator::isValidEmail(customer.Email)) {
                    InputValidator::showWarning("Email format may be invalid.");
                }
                
                customer.Address = InputValidator::getString("Address*: ", true, 5, 255);
                customer.DateOfBirth = InputValidator::getDate("Date of Birth*");
                
                if (cm.createCustomer(customer)) {
                    InputValidator::showSuccess("Customer added successfully!");
                } else {
                    InputValidator::showError("Failed to add customer. Please check the error messages above.");
                }
                InputValidator::pause();
                break;
            }
            case 2: {
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found in the database.");
                } else {
                    cm.displayAllCustomers(customers);
                }
                InputValidator::pause();
                break;
            }
            case 3: {
                searchTerm = InputValidator::getString("\nEnter search term (name, IC, phone, or email): ", false, 1, 100);
                if (!searchTerm.empty()) {
                    std::vector<Customer> customers = cm.searchCustomers(searchTerm);
                    if (customers.empty()) {
                        UIColors::printInfo("No customers found matching your search.");
                    } else {
                        cm.displayAllCustomers(customers);
                    }
                }
                InputValidator::pause();
                break;
            }
            case 4: {
                // Show list first
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found. Please add customers first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("UPDATE CUSTOMER", SCREEN_WIDTH);
                UIColors::printInfo("Please select a customer from the list below:");
                cm.displayAllCustomers(customers);
                
                customerID = InputValidator::getInt("\nEnter Customer ID to update (0 to cancel): ", 0);
                if (customerID == 0) {
                    UIColors::printInfo("Update cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Customer* existing = cm.getCustomerByID(customerID);
                if (!existing) {
                    InputValidator::showError("Customer not found.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printInfo("Current customer information:");
                cm.displayCustomer(*existing);
                UIColors::printInfo("Enter new values (press Enter to keep current value):");
                
                std::string input;
                
                std::cout << UIColors::colorize("Name", UIColors::YELLOW) << " [" << existing->Name << "]: ";
                std::getline(std::cin, input);
                customer.Name = input.empty() ? existing->Name : input;
                
                std::cout << UIColors::colorize("IC Number", UIColors::YELLOW) << " [" << existing->IC_Number << "]: ";
                std::getline(std::cin, input);
                customer.IC_Number = input.empty() ? existing->IC_Number : input;
                
                std::cout << UIColors::colorize("Phone", UIColors::YELLOW) << " [" << existing->Phone << "]: ";
                std::getline(std::cin, input);
                customer.Phone = input.empty() ? existing->Phone : input;
                
                std::cout << UIColors::colorize("Email", UIColors::YELLOW) << " [" << existing->Email << "]: ";
                std::getline(std::cin, input);
                customer.Email = input.empty() ? existing->Email : input;
                
                std::cout << UIColors::colorize("Address", UIColors::YELLOW) << " [" << existing->Address << "]: ";
                std::getline(std::cin, input);
                customer.Address = input.empty() ? existing->Address : input;
                
                std::cout << UIColors::colorize("Date of Birth", UIColors::YELLOW) << " [" << existing->DateOfBirth << "]: ";
                std::getline(std::cin, input);
                customer.DateOfBirth = input.empty() ? existing->DateOfBirth : input;
                
                if (InputValidator::confirm("Are you sure you want to update this customer?")) {
                    if (cm.updateCustomer(customerID, customer)) {
                        InputValidator::showSuccess("Customer updated successfully!");
                    } else {
                        InputValidator::showError("Failed to update customer.");
                    }
                } else {
                    UIColors::printInfo("Update cancelled.");
                }
                delete existing;
                InputValidator::pause();
                break;
            }
            case 5: {
                // Show list first
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found. Please add customers first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("DELETE CUSTOMER", SCREEN_WIDTH);
                UIColors::printInfo("Please select a customer from the list below:");
                cm.displayAllCustomers(customers);
                
                customerID = InputValidator::getInt("\nEnter Customer ID to delete (0 to cancel): ", 0);
                if (customerID == 0) {
                    UIColors::printInfo("Deletion cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Customer* customer = cm.getCustomerByID(customerID);
                if (!customer) {
                    InputValidator::showError("Customer not found.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printInfo("Customer to be deleted:");
                cm.displayCustomer(*customer);
                delete customer;
                
                if (InputValidator::confirm("Are you sure you want to delete this customer? This action cannot be undone.")) {
                    if (cm.deleteCustomer(customerID)) {
                        InputValidator::showSuccess("Customer deleted successfully!");
                    } else {
                        InputValidator::showError("Failed to delete customer.");
                    }
                } else {
                    UIColors::printInfo("Deletion cancelled.");
                }
                InputValidator::pause();
                break;
            }
            case 6: {
                // Show list first
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found. Please add customers first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("VIEW CUSTOMER DETAILS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a customer from the list below:");
                cm.displayAllCustomers(customers);
                
                customerID = InputValidator::getInt("\nEnter Customer ID (0 to cancel): ", 0);
                if (customerID == 0) {
                    UIColors::printInfo("View cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Customer* customer = cm.getCustomerByID(customerID);
                if (customer) {
                    cm.displayCustomer(*customer);
                    delete customer;
                } else {
                    InputValidator::showError("Customer not found.");
                }
                InputValidator::pause();
                break;
            }
            default:
                InputValidator::showError("Invalid choice!");
        }
    }
}

void dressManagementMenu() {
    AuthManager auth;
    if (!auth.hasPermission("Staff")) {
        UIColors::printError("Access denied. Staff or Administrator role required.");
        InputValidator::pause();
        return;
    }
    
    DressManager dm;
    int choice;
    
    while (true) {
        UIColors::printHeader("DRESS INVENTORY MANAGEMENT", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "Add New Dress");
        UIColors::printMenuOption(2, "View All Dresses");
        UIColors::printMenuOption(3, "View Available Dresses");
        UIColors::printMenuOption(4, "Search Dresses");
        UIColors::printMenuOption(5, "Update Dress");
        UIColors::printMenuOption(6, "Delete Dress");
        UIColors::printMenuOption(7, "View Dress Details");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 7);
        
        if (choice == 0) break;
        
        Dress dress;
        int dressID;
        std::string searchTerm;
        
        switch (choice) {
            case 1: {
                UIColors::printHeader("ADD NEW DRESS", SCREEN_WIDTH);
                
                dress.DressName = InputValidator::getString("Dress Name*: ", true, 2, 100);
                dress.Category = InputValidator::getString("Category*: ", true, 2, 50);
                dress.Size = InputValidator::getString("Size*: ", true, 1, 10);
                dress.Color = InputValidator::getString("Color*: ", true, 2, 30);
                dress.RentalPrice = InputValidator::getDouble("Rental Price (RM)*: ", 0.01, 10000.0);
                
                std::cout << UIColors::colorize("Condition Status", UIColors::YELLOW) << " (Good/Fair/Poor) [Good]: ";
                std::string cond;
                std::getline(std::cin, cond);
                dress.ConditionStatus = cond.empty() ? "Good" : cond;
                
                std::cout << UIColors::colorize("Availability Status", UIColors::YELLOW) << " (Available/Rented/Maintenance) [Available]: ";
                std::string avail;
                std::getline(std::cin, avail);
                dress.AvailabilityStatus = avail.empty() ? "Available" : avail;
                
                if (dm.createDress(dress)) {
                    InputValidator::showSuccess("Dress added successfully!");
                } else {
                    InputValidator::showError("Failed to add dress.");
                }
                InputValidator::pause();
                break;
            }
            case 2: {
                std::vector<Dress> dresses = dm.getAllDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No dresses found in the database.");
                } else {
                    dm.displayAllDresses(dresses);
                }
                InputValidator::pause();
                break;
            }
            case 3: {
                std::vector<Dress> dresses = dm.getAvailableDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No available dresses found.");
                } else {
                    dm.displayAllDresses(dresses);
                }
                InputValidator::pause();
                break;
            }
            case 4: {
                searchTerm = InputValidator::getString("\nEnter search term: ", false, 1, 100);
                if (!searchTerm.empty()) {
                    std::vector<Dress> dresses = dm.searchDresses(searchTerm);
                    if (dresses.empty()) {
                        UIColors::printInfo("No dresses found matching your search.");
                    } else {
                        dm.displayAllDresses(dresses);
                    }
                }
                InputValidator::pause();
                break;
            }
            case 5: {
                // Show list first
                std::vector<Dress> dresses = dm.getAllDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No dresses found. Please add dresses first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("UPDATE DRESS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a dress from the list below:");
                dm.displayAllDresses(dresses);
                
                dressID = InputValidator::getInt("\nEnter Dress ID to update (0 to cancel): ", 0);
                if (dressID == 0) {
                    UIColors::printInfo("Update cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Dress* existing = dm.getDressByID(dressID);
                if (!existing) {
                    InputValidator::showError("Dress not found.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printInfo("Current dress information:");
                dm.displayDress(*existing);
                UIColors::printInfo("Enter new values (press Enter to keep current value):");
                
                std::string input;
                
                std::cout << UIColors::colorize("Dress Name", UIColors::YELLOW) << " [" << existing->DressName << "]: ";
                std::getline(std::cin, input);
                dress.DressName = input.empty() ? existing->DressName : input;
                
                std::cout << UIColors::colorize("Category", UIColors::YELLOW) << " [" << existing->Category << "]: ";
                std::getline(std::cin, input);
                dress.Category = input.empty() ? existing->Category : input;
                
                std::cout << UIColors::colorize("Size", UIColors::YELLOW) << " [" << existing->Size << "]: ";
                std::getline(std::cin, input);
                dress.Size = input.empty() ? existing->Size : input;
                
                std::cout << UIColors::colorize("Color", UIColors::YELLOW) << " [" << existing->Color << "]: ";
                std::getline(std::cin, input);
                dress.Color = input.empty() ? existing->Color : input;
                
                std::cout << UIColors::colorize("Rental Price", UIColors::YELLOW) << " [" << existing->RentalPrice << "]: ";
                std::getline(std::cin, input);
                dress.RentalPrice = input.empty() ? existing->RentalPrice : std::stod(input);
                
                std::cout << UIColors::colorize("Condition Status", UIColors::YELLOW) << " [" << existing->ConditionStatus << "]: ";
                std::getline(std::cin, input);
                dress.ConditionStatus = input.empty() ? existing->ConditionStatus : input;
                
                std::cout << UIColors::colorize("Availability Status", UIColors::YELLOW) << " [" << existing->AvailabilityStatus << "]: ";
                std::getline(std::cin, input);
                dress.AvailabilityStatus = input.empty() ? existing->AvailabilityStatus : input;
                
                if (InputValidator::confirm("Are you sure you want to update this dress?")) {
                    if (dm.updateDress(dressID, dress)) {
                        InputValidator::showSuccess("Dress updated successfully!");
                    } else {
                        InputValidator::showError("Failed to update dress.");
                    }
                } else {
                    UIColors::printInfo("Update cancelled.");
                }
                delete existing;
                InputValidator::pause();
                break;
            }
            case 6: {
                // Show list first
                std::vector<Dress> dresses = dm.getAllDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No dresses found. Please add dresses first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("DELETE DRESS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a dress from the list below:");
                dm.displayAllDresses(dresses);
                
                dressID = InputValidator::getInt("\nEnter Dress ID to delete (0 to cancel): ", 0);
                if (dressID == 0) {
                    UIColors::printInfo("Deletion cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Dress* dress = dm.getDressByID(dressID);
                if (!dress) {
                    InputValidator::showError("Dress not found.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printInfo("Dress to be deleted:");
                dm.displayDress(*dress);
                delete dress;
                
                if (InputValidator::confirm("Are you sure you want to delete this dress? This action cannot be undone.")) {
                    if (dm.deleteDress(dressID)) {
                        InputValidator::showSuccess("Dress deleted successfully!");
                    } else {
                        InputValidator::showError("Failed to delete dress.");
                    }
                } else {
                    UIColors::printInfo("Deletion cancelled.");
                }
                InputValidator::pause();
                break;
            }
            case 7: {
                // Show list first
                std::vector<Dress> dresses = dm.getAllDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No dresses found. Please add dresses first.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printHeader("VIEW DRESS DETAILS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a dress from the list below:");
                dm.displayAllDresses(dresses);
                
                dressID = InputValidator::getInt("\nEnter Dress ID (0 to cancel): ", 0);
                if (dressID == 0) {
                    UIColors::printInfo("View cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Dress* dress = dm.getDressByID(dressID);
                if (dress) {
                    dm.displayDress(*dress);
                    delete dress;
                } else {
                    InputValidator::showError("Dress not found.");
                }
                InputValidator::pause();
                break;
            }
            default:
                InputValidator::showError("Invalid choice!");
        }
    }
}

void rentalManagementMenu() {
    AuthManager auth;
    if (!auth.hasPermission("Staff")) {
        UIColors::printError("Access denied. Staff or Administrator role required.");
        InputValidator::pause();
        return;
    }
    
    RentalManager rm;
    PaymentManager pm;
    CustomerManager cm;
    DressManager dm;
    int choice;
    
    while (true) {
        UIColors::printHeader("RENTAL TRANSACTION MANAGEMENT", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "Create New Rental");
        UIColors::printMenuOption(2, "View All Active Rentals");
        UIColors::printMenuOption(3, "View Overdue Rentals");
        UIColors::printMenuOption(4, "View Rental Details");
        UIColors::printMenuOption(5, "Return Rental");
        UIColors::printMenuOption(6, "View Customer Rentals");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 6);
        
        if (choice == 0) break;
        
        int customerID, rentalID, duration;
        std::string rentalDate, returnDate;
        std::vector<int> dressIDs;
        
        switch (choice) {
            case 1: {
                UIColors::printHeader("CREATE NEW RENTAL", SCREEN_WIDTH);
                
                // Show customers first
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found. Please add customers first.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printInfo("Please select a customer from the list below:");
                cm.displayAllCustomers(customers);
                
                customerID = InputValidator::getInt("\nCustomer ID* (0 to cancel): ", 0);
                if (customerID == 0) {
                    UIColors::printInfo("Rental creation cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Customer* customer = cm.getCustomerByID(customerID);
                if (!customer) {
                    InputValidator::showError("Customer not found. Please create the customer first.");
                    InputValidator::pause();
                    break;
                }
                delete customer;
                
                rentalDate = InputValidator::getDate("Rental Date*");
                duration = InputValidator::getInt("Duration (1-14 days)*: ", 1, 14);
                
                // Show available dresses
                std::vector<Dress> availableDresses = dm.getAvailableDresses();
                if (availableDresses.empty()) {
                    UIColors::printInfo("No available dresses found. Please add dresses first.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printInfo("Please select dresses from the available list below:");
                dm.displayAllDresses(availableDresses);
                
                std::cout << "\nEnter dress IDs (1-5 dresses, enter 0 to finish): " << std::endl;
                int dressID;
                while (dressIDs.size() < 5) {
                    dressID = InputValidator::getInt("Dress ID " + std::to_string(dressIDs.size() + 1) + " (0 to finish): ", 0);
                    if (dressID == 0) break;
                    dressIDs.push_back(dressID);
                }
                
                if (dressIDs.empty()) {
                    InputValidator::showError("At least one dress must be selected.");
                    InputValidator::pause();
                    break;
                }
                
                int newRentalID = rm.createRental(customerID, rentalDate, duration, dressIDs);
                if (newRentalID > 0) {
                    InputValidator::showSuccess("Rental created successfully! Rental ID: " + std::to_string(newRentalID));
                    UIColors::printInfo("Please process payment before rental can commence.");
                } else {
                    InputValidator::showError("Failed to create rental. Please check the error messages above.");
                }
                InputValidator::pause();
                break;
            }
            case 2: {
                std::vector<Rental> rentals = rm.getActiveRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No active rentals found.");
                } else {
                    for (const auto& rental : rentals) {
                        rm.displayRental(rental);
                    }
                }
                InputValidator::pause();
                break;
            }
            case 3: {
                std::vector<Rental> rentals = rm.getOverdueRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No overdue rentals found.");
                } else {
                    for (const auto& rental : rentals) {
                        rm.displayRental(rental);
                    }
                }
                InputValidator::pause();
                break;
            }
            case 4: {
                // Show rentals first
                std::vector<Rental> rentals = rm.getActiveRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No active rentals found.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("VIEW RENTAL DETAILS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a rental from the list below:");
                for (const auto& rental : rentals) {
                    rm.displayRental(rental);
                }
                
                rentalID = InputValidator::getInt("\nEnter Rental ID (0 to cancel): ", 0);
                if (rentalID == 0) {
                    UIColors::printInfo("View cancelled.");
                    InputValidator::pause();
                    break;
                }
                rm.displayRentalDetails(rentalID);
                InputValidator::pause();
                break;
            }
            case 5: {
                // Show active rentals first
                std::vector<Rental> rentals = rm.getActiveRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No active rentals to return.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("RETURN RENTAL", SCREEN_WIDTH);
                UIColors::printInfo("Please select a rental from the list below:");
                for (const auto& rental : rentals) {
                    rm.displayRental(rental);
                }
                
                rentalID = InputValidator::getInt("\nEnter Rental ID to return (0 to cancel): ", 0);
                if (rentalID == 0) {
                    UIColors::printInfo("Return cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Rental* rental = rm.getRentalByID(rentalID);
                if (!rental) {
                    InputValidator::showError("Rental not found.");
                    InputValidator::pause();
                    break;
                }
                
                if (rental->Status != "Active") {
                    UIColors::printWarning("This rental is already returned.");
                    delete rental;
                    InputValidator::pause();
                    break;
                }
                
                returnDate = InputValidator::getDate("Return Date*");
                
                if (InputValidator::confirm("Are you sure you want to return this rental?")) {
                    if (rm.returnRental(rentalID, returnDate)) {
                        InputValidator::showSuccess("Rental returned successfully!");
                    } else {
                        InputValidator::showError("Failed to return rental.");
                    }
                } else {
                    UIColors::printInfo("Return cancelled.");
                }
                delete rental;
                InputValidator::pause();
                break;
            }
            case 6: {
                // Show customers first
                std::vector<Customer> customers = cm.getAllCustomers();
                if (customers.empty()) {
                    UIColors::printInfo("No customers found.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("VIEW CUSTOMER RENTALS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a customer from the list below:");
                cm.displayAllCustomers(customers);
                
                customerID = InputValidator::getInt("\nEnter Customer ID (0 to cancel): ", 0);
                if (customerID == 0) {
                    UIColors::printInfo("View cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                std::vector<Rental> rentals = rm.getRentalsByCustomer(customerID);
                if (rentals.empty()) {
                    UIColors::printInfo("No rentals found for this customer.");
                } else {
                    for (const auto& rental : rentals) {
                        rm.displayRental(rental);
                    }
                }
                InputValidator::pause();
                break;
            }
            default:
                InputValidator::showError("Invalid choice!");
        }
    }
}

void paymentManagementMenu() {
    AuthManager auth;
    if (!auth.hasPermission("Staff")) {
        UIColors::printError("Access denied. Staff or Administrator role required.");
        InputValidator::pause();
        return;
    }
    
    PaymentManager pm;
    RentalManager rm;
    int choice;
    
    while (true) {
        UIColors::printHeader("PAYMENT PROCESSING", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "Process Payment");
        UIColors::printMenuOption(2, "View All Payments");
        UIColors::printMenuOption(3, "View Payments by Rental");
        UIColors::printMenuOption(4, "Generate Receipt");
        UIColors::printMenuOption(5, "Update Payment Status");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 5);
        
        if (choice == 0) break;
        
        int rentalID, paymentID;
        double amount;
        std::string paymentMethod, paymentDate, status, transactionRef;
        
        switch (choice) {
            case 1: {
                UIColors::printHeader("PROCESS PAYMENT", SCREEN_WIDTH);
                
                // Show active rentals first
                std::vector<Rental> rentals = rm.getActiveRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No active rentals found. Please create a rental first.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printInfo("Please select a rental from the list below:");
                for (const auto& rental : rentals) {
                    rm.displayRental(rental);
                }
                
                rentalID = InputValidator::getInt("\nRental ID* (0 to cancel): ", 0);
                if (rentalID == 0) {
                    UIColors::printInfo("Payment cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Rental* rental = rm.getRentalByID(rentalID);
                if (!rental) {
                    InputValidator::showError("Rental not found.");
                    InputValidator::pause();
                    break;
                }
                
                double totalDue = rental->TotalAmount + rental->LateFee;
                double totalPaid = pm.getTotalPaid(rentalID);
                double remaining = totalDue - totalPaid;
                
                UIColors::printInfo("Payment Summary:");
                std::cout << "Total Due: " << UIColors::colorize("RM " + std::to_string(totalDue), UIColors::RED) << std::endl;
                std::cout << "Total Paid: " << UIColors::colorize("RM " + std::to_string(totalPaid), UIColors::GREEN) << std::endl;
                std::cout << "Remaining: " << UIColors::colorize("RM " + std::to_string(remaining), UIColors::YELLOW) << std::endl;
                
                if (remaining <= 0) {
                    UIColors::printInfo("This rental is already fully paid.");
                    delete rental;
                    InputValidator::pause();
                    break;
                }
                
                amount = InputValidator::getDouble("\nPayment Amount (RM)*: ", 0.01, remaining);
                std::cout << UIColors::colorize("Payment Method", UIColors::YELLOW) << " (Cash/Credit Card/Debit Card/Online)*: ";
                std::getline(std::cin, paymentMethod);
                paymentDate = InputValidator::getDate("Payment Date*");
                transactionRef = InputValidator::getString("Transaction Reference (optional): ", false, 0, 100);
                
                if (InputValidator::confirm("Confirm payment processing?")) {
                    if (pm.createPayment(rentalID, amount, paymentMethod, paymentDate, transactionRef)) {
                        InputValidator::showSuccess("Payment processed successfully!");
                    } else {
                        InputValidator::showError("Failed to process payment.");
                    }
                } else {
                    UIColors::printInfo("Payment cancelled.");
                }
                delete rental;
                InputValidator::pause();
                break;
            }
            case 2: {
                std::vector<Payment> payments = pm.getAllPayments();
                if (payments.empty()) {
                    UIColors::printInfo("No payments found.");
                } else {
                    pm.displayAllPayments(payments);
                }
                InputValidator::pause();
                break;
            }
            case 3: {
                // Show rentals first
                std::vector<Rental> rentals = rm.getActiveRentals();
                if (rentals.empty()) {
                    UIColors::printInfo("No active rentals found.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("VIEW PAYMENTS BY RENTAL", SCREEN_WIDTH);
                UIColors::printInfo("Please select a rental from the list below:");
                for (const auto& rental : rentals) {
                    rm.displayRental(rental);
                }
                
                rentalID = InputValidator::getInt("\nEnter Rental ID (0 to cancel): ", 0);
                if (rentalID == 0) {
                    UIColors::printInfo("View cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                std::vector<Payment> payments = pm.getPaymentsByRental(rentalID);
                if (payments.empty()) {
                    UIColors::printInfo("No payments found for this rental.");
                } else {
                    pm.displayAllPayments(payments);
                }
                InputValidator::pause();
                break;
            }
            case 4: {
                // Show payments first
                std::vector<Payment> payments = pm.getAllPayments();
                if (payments.empty()) {
                    UIColors::printInfo("No payments found.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("GENERATE RECEIPT", SCREEN_WIDTH);
                UIColors::printInfo("Please select a payment from the list below:");
                pm.displayAllPayments(payments);
                
                paymentID = InputValidator::getInt("\nEnter Payment ID (0 to cancel): ", 0);
                if (paymentID == 0) {
                    UIColors::printInfo("Receipt generation cancelled.");
                    InputValidator::pause();
                    break;
                }
                pm.generateReceipt(paymentID);
                InputValidator::pause();
                break;
            }
            case 5: {
                // Show payments first
                std::vector<Payment> payments = pm.getAllPayments();
                if (payments.empty()) {
                    UIColors::printInfo("No payments found.");
                    InputValidator::pause();
                    break;
                }
                UIColors::printHeader("UPDATE PAYMENT STATUS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a payment from the list below:");
                pm.displayAllPayments(payments);
                
                paymentID = InputValidator::getInt("\nEnter Payment ID (0 to cancel): ", 0);
                if (paymentID == 0) {
                    UIColors::printInfo("Update cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Payment* payment = pm.getPaymentByID(paymentID);
                if (!payment) {
                    InputValidator::showError("Payment not found.");
                    InputValidator::pause();
                    break;
                }
                delete payment;
                
                std::cout << UIColors::colorize("New Status", UIColors::YELLOW) << " (Pending/Completed/Failed/Refunded)*: ";
                std::getline(std::cin, status);
                
                if (InputValidator::confirm("Are you sure you want to update the payment status?")) {
                    if (pm.updatePaymentStatus(paymentID, status)) {
                        InputValidator::showSuccess("Payment status updated successfully!");
                    } else {
                        InputValidator::showError("Failed to update payment status.");
                    }
                } else {
                    UIColors::printInfo("Update cancelled.");
                }
                InputValidator::pause();
                break;
            }
            default:
                InputValidator::showError("Invalid choice!");
        }
    }
}

void reportsMenu() {
    AuthManager auth;
    if (!auth.hasPermission("Staff")) {
        UIColors::printError("Access denied. Staff or Administrator role required.");
        InputValidator::pause();
        return;
    }
    
    ReportManager rm;
    int choice;
    
    while (true) {
        UIColors::printHeader("REPORTS & ANALYTICS", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "Monthly Sales Report");
        UIColors::printMenuOption(2, "Inventory Valuation Report");
        UIColors::printMenuOption(3, "Dress Utilization Report");
        UIColors::printMenuOption(4, "Customer Activity Report");
        UIColors::printMenuOption(5, "Overdue Items Report");
        UIColors::printMenuOption(6, "Rental Summary Report");
        UIColors::printMenuOption(7, "Income Statement");
        UIColors::printMenuOption(8, "Customer Loyalty Report");
        UIColors::printMenuOption(9, "Profit Margin Report");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 9);
        
        if (choice == 0) break;
        
        std::string year, startDate, endDate;
        
        switch (choice) {
            case 1:
                year = InputValidator::getString("\nEnter year (YYYY): ", true, 4, 4);
                if (InputValidator::isValidDate(year + "-01-01")) {
                    rm.generateMonthlySalesReport(year);
                } else {
                    InputValidator::showError("Invalid year format.");
                }
                InputValidator::pause();
                break;
            case 2:
                rm.generateInventoryValuationReport();
                InputValidator::pause();
                break;
            case 3:
                rm.generateDressUtilizationReport();
                InputValidator::pause();
                break;
            case 4:
                rm.generateCustomerActivityReport();
                InputValidator::pause();
                break;
            case 5:
                rm.generateOverdueItemsReport();
                InputValidator::pause();
                break;
            case 6:
                rm.generateRentalSummaryReport();
                InputValidator::pause();
                break;
            case 7:
                startDate = InputValidator::getDate("Start Date*");
                endDate = InputValidator::getDate("End Date*");
                if (!startDate.empty() && !endDate.empty() && startDate <= endDate) {
                    rm.generateIncomeStatement(startDate, endDate);
                } else {
                    InputValidator::showError("Invalid date range. Start date must be before or equal to end date.");
                }
                InputValidator::pause();
                break;
            case 8:
                rm.generateCustomerLoyaltyReport();
                InputValidator::pause();
                break;
            case 9:
                rm.generateProfitMarginReport();
                InputValidator::pause();
                break;
            default:
                InputValidator::showError("Invalid choice!");
        }
    }
}

void userManagementMenu() {
    AuthManager auth;
    if (!auth.hasPermission("Administrator")) {
        UIColors::printError("Access denied. Administrator role required.");
        InputValidator::pause();
        return;
    }
    
    int choice;
    while (true) {
        UIColors::printHeader("USER MANAGEMENT", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "View All Users");
        UIColors::printMenuOption(2, "Add New User");
        UIColors::printMenuOption(3, "Update User");
        UIColors::printMenuOption(4, "Deactivate User");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 4);
        if (choice == 0) break;
        
        // User management implementation here
        UIColors::printInfo("User management features - to be implemented");
        InputValidator::pause();
    }
}

void customerViewMenu() {
    AuthManager auth;
    User* user = auth.getCurrentUser();
    if (!user || user->Role != "Customer") {
        UIColors::printError("Access denied. Customer role required.");
        InputValidator::pause();
        return;
    }
    
    RentalManager rm;
    DressManager dm;
    int choice;
    
    while (true) {
        UIColors::printHeader("CUSTOMER PORTAL", SCREEN_WIDTH);
        UIColors::printMenuOption(1, "View My Rentals");
        UIColors::printMenuOption(2, "View Available Dresses");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        choice = InputValidator::getInt("Enter your choice: ", 0, 2);
        if (choice == 0) break;
        
        switch (choice) {
            case 1: {
                // Get customer ID from user (would be linked in real system)
                int customerID = InputValidator::getInt("Enter your Customer ID: ", 1);
                std::vector<Rental> rentals = rm.getRentalsByCustomer(customerID);
                if (rentals.empty()) {
                    UIColors::printInfo("No rentals found.");
                } else {
                    for (const auto& rental : rentals) {
                        rm.displayRental(rental);
                    }
                }
                InputValidator::pause();
                break;
            }
            case 2: {
                std::vector<Dress> dresses = dm.getAvailableDresses();
                if (dresses.empty()) {
                    UIColors::printInfo("No available dresses found.");
                } else {
                    dm.displayAllDresses(dresses);
                }
                InputValidator::pause();
                break;
            }
        }
    }
}

