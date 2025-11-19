#include <iostream>
#include <string>
#include "DatabaseManager.h"
#include "InputValidator.h"
#include "UIColors.h"
#include "AuthManager.h"
#include "ReportManager.h"
#include "DressManager.h"
#include "MenuHandlers.h"

int main() {
    UIColors::enableColors();
    
    UIColors::printSeparator(SCREEN_WIDTH);
    UIColors::printCentered("Initializing Dress Rental Management System...", SCREEN_WIDTH, UIColors::CYAN);
    UIColors::printSeparator(SCREEN_WIDTH);
    
    try {
        if (!DatabaseManager::getInstance().connect()) {
            UIColors::printError("Failed to connect to database. Please ensure MySQL is running and database exists.");
            UIColors::printInfo("Database: dress_rental_db, User: root, Password: (empty)");
            InputValidator::pause();
            return 1;
        }
        
        UIColors::printSuccess("Connected to database successfully!");
    } catch (const std::exception& e) {
        UIColors::printError("Database connection error: " + std::string(e.what()));
        InputValidator::pause();
        return 1;
    }
    
    // Require login
    if (!showLoginScreen()) {
        DatabaseManager::getInstance().disconnect();
        return 1;
    }
    
    AuthManager auth;
    int choice;
    
    while (true) {
        displayMainMenu();
        
        std::string input;
        std::getline(std::cin, input);
        
        try {
            if (input.empty()) {
                UIColors::printError("Please enter a choice.");
                continue;
            }
            choice = std::stoi(input);
        } catch (const std::exception&) {
            InputValidator::clearInputBuffer();
            UIColors::printError("Invalid input. Please enter a valid number.");
            continue;
        }
        
        if (choice == 0) {
            if (InputValidator::confirm("\nAre you sure you want to logout?")) {
                auth.logout();
                UIColors::printCentered("Logged out successfully!", SCREEN_WIDTH, UIColors::GREEN);
                if (!showLoginScreen()) {
                    break;
                }
            }
            continue;
        }
        
        try {
            // Dynamic menu handling based on role
            int menuOffset = 0;
            
            if (auth.hasPermission("Staff")) {
                if (choice == 1) { customerManagementMenu(); continue; }
                if (choice == 2) { dressManagementMenu(); continue; }
                if (choice == 3) { rentalManagementMenu(); continue; }
                if (choice == 4) { paymentManagementMenu(); continue; }
                if (choice == 5) { reportsMenu(); continue; }
                if (choice == 6) {
                    ReportManager rm;
                    rm.displayDashboard();
                    InputValidator::pause();
                    continue;
                }
                menuOffset = 6;
            }
            
            if (auth.hasPermission("Administrator")) {
                if (choice == 7) { userManagementMenu(); continue; }
                menuOffset = 7;
            }
            
            if (auth.hasPermission("Customer")) {
                if (choice == 8) { customerViewMenu(); continue; }
                if (choice == 9) {
                    DressManager dm;
                    std::vector<Dress> dresses = dm.getAvailableDresses();
                    if (dresses.empty()) {
                        UIColors::printInfo("No available dresses found.");
                    } else {
                        dm.displayAllDresses(dresses);
                    }
                    InputValidator::pause();
                    continue;
                }
            }
            
            // Change Password (available to all)
            if (choice == menuOffset + 1 || (auth.hasPermission("Customer") && choice == 10)) {
                showChangePasswordMenu();
                continue;
            }
            
            UIColors::printError("Invalid choice! Please try again.");
        } catch (const std::exception& e) {
            UIColors::printError("An error occurred: " + std::string(e.what()));
            InputValidator::pause();
        }
    }
    
    auth.logout();
    DatabaseManager::getInstance().disconnect();
    return 0;
}
