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
            // Common menu options (1-4) for both Staff and Administrator
            if (auth.hasPermission("Staff")) {
                if (choice == 1) { customerManagementMenu(); continue; }
                if (choice == 2) { dressManagementMenu(); continue; }
                if (choice == 3) { rentalManagementMenu(); continue; }
                if (choice == 4) { paymentManagementMenu(); continue; }
            }
            
            // Administrator only options (5-6)
            if (auth.hasPermission("Administrator")) {
                if (choice == 5) { reportsMenu(); continue; }
                if (choice == 6) {
                    ReportManager rm;
                    rm.displayDashboard();
                    InputValidator::pause();
                    continue;
                }
            } else {
                // Staff access denied for options 5-6
                if (choice == 5) {
                    UIColors::printError("Access denied. Administrator role required to view Reports & Analytics.");
                    UIColors::printCentered("Please contact your administrator for access.", SCREEN_WIDTH, UIColors::YELLOW);
                    InputValidator::pause();
                    continue;
                }
                if (choice == 6) {
                    UIColors::printError("Access denied. Administrator role required to view Dashboard.");
                    UIColors::printCentered("Please contact your administrator for access.", SCREEN_WIDTH, UIColors::YELLOW);
                    InputValidator::pause();
                    continue;
                }
            }
            
            // Change Password (always option 7 for both Staff and Administrator)
            if (choice == 7) { 
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
