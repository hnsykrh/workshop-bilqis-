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
        std::cout << std::endl;
        UIColors::printSeparator(SCREEN_WIDTH);
        UIColors::printCentered("DRESS RENTAL MANAGEMENT SYSTEM", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
        UIColors::printCentered("Please Login to Continue", SCREEN_WIDTH, UIColors::YELLOW);
        UIColors::printCentered("(Admin and Staff Only)", SCREEN_WIDTH, UIColors::DIM + UIColors::WHITE);
        UIColors::printSeparator(SCREEN_WIDTH);
        std::cout << std::endl;
        
        // Sign Up option
        int centerPos = SCREEN_WIDTH / 2;
        UIColors::printCentered("1. Login", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("2. Sign Up (Create New Staff Account)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printSeparator(SCREEN_WIDTH);
        std::cout << std::endl;
        
        UIColors::printCenteredInput("Select option (1 or 2): ", SCREEN_WIDTH, UIColors::WHITE);
        int option = InputValidator::getInt("", 1, 2, true);
        
        // Handle Sign Up
        if (option == 2) {
            UIColors::printHeader("SIGN UP - CREATE NEW STAFF ACCOUNT", SCREEN_WIDTH);
            
            User newUser;
            newUser.Username = InputValidator::getString("Username*: ", true, 3, 50);
            
            // Check if username already exists
            try {
                sql::Connection* conn = DatabaseManager::getInstance().getConnection();
                if (conn) {
                    sql::PreparedStatement* pstmt = conn->prepareStatement(
                        "SELECT COUNT(*) as count FROM Users WHERE Username = ?"
                    );
                    pstmt->setString(1, newUser.Username);
                    sql::ResultSet* res = pstmt->executeQuery();
                    if (res && res->next() && res->getInt("count") > 0) {
                        UIColors::printCentered("Username already exists! Please choose another username.", SCREEN_WIDTH, UIColors::RED);
                        delete pstmt;
                        if (res) delete res;
                        InputValidator::pause();
                        continue;
                    }
                    delete pstmt;
                    if (res) delete res;
                }
            } catch (sql::SQLException& e) {
                UIColors::printCentered("Error checking username: " + std::string(e.what()), SCREEN_WIDTH, UIColors::RED);
                InputValidator::pause();
                continue;
            }
            
            std::string password = InputValidator::getPassword("Password*: ", true);
            std::string confirmPassword = InputValidator::getPassword("Confirm Password*: ", false);
            
            if (password != confirmPassword) {
                UIColors::printCentered("Passwords do not match!", SCREEN_WIDTH, UIColors::RED);
                InputValidator::pause();
                continue;
            }
            
            newUser.PasswordHash = password; // Will be hashed in createUser
            newUser.Role = "Staff"; // All new users get Staff role
            newUser.FullName = InputValidator::getString("Full Name*: ", true, 2, 100);
            newUser.Email = InputValidator::getString("Email: ", false, 0, 100);
            newUser.Phone = InputValidator::getString("Phone: ", false, 0, 20);
            newUser.IsActive = true;
            
            if (auth.createUser(newUser)) {
                UIColors::printCentered("Account created successfully! You can now login.", SCREEN_WIDTH, UIColors::GREEN);
                InputValidator::pause();
                continue; // Return to login screen
            } else {
                UIColors::printCentered("Failed to create account. Please try again.", SCREEN_WIDTH, UIColors::RED);
                InputValidator::pause();
                continue;
            }
        }
        
        // Continue with normal login if option 1
        if (option != 1) {
            // Already handled sign up above, continue to login loop
            continue;
        }
        
        // Calculate center position for input
        int promptLen = 10; // "Username: " length
        int startPos = centerPos - (promptLen / 2);
        
        std::cout << std::string(startPos, ' ') << UIColors::colorize("Username: ", UIColors::WHITE) << std::flush;
        std::string username = InputValidator::getString("", true, 3, 50);
        std::cout << std::endl;
        
        promptLen = 10; // "Password: " length
        startPos = centerPos - (promptLen / 2);
        std::cout << std::string(startPos, ' ') << UIColors::colorize("Password: ", UIColors::WHITE) << std::flush;
        std::string password = InputValidator::getPassword("", false, true);  // Skip validation for login
        
        if (auth.login(username, password)) {
            User* user = auth.getCurrentUser();
            std::cout << std::endl;
            UIColors::printCentered("Login successful! Welcome, " + user->FullName + "!", SCREEN_WIDTH, UIColors::GREEN);
            UIColors::printCentered("Role: " + user->Role, SCREEN_WIDTH, UIColors::CYAN);
            std::cout << std::endl;
            
            // Show low stock alert
            std::vector<std::pair<int, std::string>> lowStock = auth.getLowStockDresses(3);
            if (!lowStock.empty()) {
                std::cout << std::endl;
                UIColors::printCentered("*** LOW STOCK ALERT ***", SCREEN_WIDTH, UIColors::YELLOW + UIColors::BOLD);
                UIColors::printSeparator(SCREEN_WIDTH);
                UIColors::printCentered("The following dresses have low stock:", SCREEN_WIDTH, UIColors::YELLOW);
                for (const auto& item : lowStock) {
                    std::string itemText = "- Dress ID " + std::to_string(item.first) + ": " + item.second;
                    UIColors::printCentered(itemText, SCREEN_WIDTH, UIColors::WHITE);
                }
                UIColors::printSeparator(SCREEN_WIDTH);
            }
            
            std::cout << std::endl;
            UIColors::printCenteredInput("Press Enter to continue...", SCREEN_WIDTH, UIColors::WHITE);
            InputValidator::pause("");
            return true;
        } else {
            attempts++;
            std::cout << std::endl;
            UIColors::printCentered("Invalid username or password. Attempts remaining: " + std::to_string(MAX_ATTEMPTS - attempts), SCREEN_WIDTH, UIColors::RED);
            std::cout << std::endl;
            if (attempts < MAX_ATTEMPTS) {
                InputValidator::pause("Press Enter to try again...");
            }
        }
    }
    
    std::cout << std::endl;
    UIColors::printCentered("Maximum login attempts reached. System will exit.", SCREEN_WIDTH, UIColors::RED);
    std::cout << std::endl;
    InputValidator::pause();
    return false;
}

void showChangePasswordMenu() {
    AuthManager auth;
    User* user = auth.getCurrentUser();
    
    if (!user) {
        UIColors::printCentered("No user logged in.", SCREEN_WIDTH, UIColors::RED);
        return;
    }
    
    UIColors::printHeader("CHANGE PASSWORD", SCREEN_WIDTH);
    
    UIColors::printCentered("Enter current password:", SCREEN_WIDTH, UIColors::WHITE);
    std::string oldPassword = InputValidator::getPassword("", false, true);  // Skip validation for current password
    
    std::string newPassword = InputValidator::getPassword("Enter new password: ", true);
    UIColors::printCentered("Confirm new password:", SCREEN_WIDTH, UIColors::WHITE);
    std::string confirmPassword = InputValidator::getPassword("", false, true);  // Skip validation for confirmation (only validate the new password)
    
    if (newPassword != confirmPassword) {
        UIColors::printCentered("New passwords do not match!", SCREEN_WIDTH, UIColors::RED);
        InputValidator::pause();
        return;
    }
    
    if (auth.changePassword(user->UserID, oldPassword, newPassword)) {
        UIColors::printCentered("Password changed successfully!", SCREEN_WIDTH, UIColors::GREEN);
    } else {
        UIColors::printCentered("Failed to change password. Please check your current password.", SCREEN_WIDTH, UIColors::RED);
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
    
    // Staff and Administrator options (1-4)
    if (auth.hasPermission("Staff")) {
        UIColors::printMenuOption(menuNum++, "Customer Management");
        UIColors::printMenuOption(menuNum++, "Dress Inventory Management");
        UIColors::printMenuOption(menuNum++, "Rental Transaction Management");
        UIColors::printMenuOption(menuNum++, "Payment Processing");
    }
    
    // Administrator only options (5-6)
    if (auth.hasPermission("Administrator")) {
        UIColors::printMenuOption(menuNum++, "Reports & Analytics");
        UIColors::printMenuOption(menuNum++, "Dashboard");
    }
    
    // Change Password (always option 7)
    std::cout << std::endl;
    UIColors::printMenuOption(7, "Change Password");
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
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 6);
        
        if (choice == 0) break;
        
        Customer customer;
        int customerID;
        std::string searchTerm;
        
        switch (choice) {
            case 1: {
                UIColors::printHeader("ADD NEW CUSTOMER", SCREEN_WIDTH);
                UIColors::printInfo("Please fill in all required fields (*).");
                std::cout << std::endl;
                
                // Customer Information
                UIColors::printCentered("=== CUSTOMER INFORMATION ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                std::cout << std::endl;
                
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
                
                // Create customer first
                if (!cm.createCustomer(customer)) {
                    InputValidator::showError("Failed to add customer. Please check the error messages above.");
                    InputValidator::pause();
                    break;
                }
                
                InputValidator::showSuccess("Customer added successfully!");
                std::cout << std::endl;
                
                // Dress Order Details
                UIColors::printCentered("=== DRESS ORDER DETAILS ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                std::cout << std::endl;
                UIColors::printCentered("Would you like to add a dress order for this customer?", SCREEN_WIDTH, UIColors::YELLOW);
                
                if (InputValidator::confirm("Add dress order?")) {
                    DressManager dm;
                    RentalManager rm;
                    
                    // Get customer ID (just created)
                    Customer* newCustomer = cm.getCustomerByIC(customer.IC_Number);
                    if (!newCustomer) {
                        InputValidator::showError("Could not retrieve customer information.");
                        InputValidator::pause();
                        break;
                    }
                    int newCustomerID = newCustomer->CustomerID;
                    delete newCustomer;
                    
                    // Show available dresses
                    std::vector<Dress> availableDresses = dm.getAvailableDresses();
                    if (availableDresses.empty()) {
                        UIColors::printInfo("No available dresses found. Please add dresses first.");
                        InputValidator::pause();
                        break;
                    }
                    
                    UIColors::printInfo("Please select a dress from the available list below:");
                    dm.displayAllDresses(availableDresses);
                    std::cout << std::endl;
                    
                    int dressID = InputValidator::getInt("Enter Dress ID*: ", 1);
                    Dress* selectedDress = dm.getDressByID(dressID);
                    if (!selectedDress) {
                        InputValidator::showError("Dress not found.");
                        InputValidator::pause();
                        break;
                    }
                    
                    std::string rentalDate = InputValidator::getDate("Rental Date* (when is the rent wanted): ");
                    int duration = InputValidator::getInt("Rental Duration (1-14 days)*: ", 1, 14);
                    
                    // Calculate total price
                    double totalPrice = selectedDress->RentalPrice * duration;
                    
                    // Display order summary
                    std::cout << std::endl;
                    UIColors::printCentered("=== ORDER SUMMARY ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                    std::cout << std::endl;
                    UIColors::printCentered("Customer: " + customer.Name, SCREEN_WIDTH, UIColors::WHITE);
                    UIColors::printCentered("Dress: " + selectedDress->DressName, SCREEN_WIDTH, UIColors::WHITE);
                    std::ostringstream priceStream;
                    priceStream << std::fixed << std::setprecision(2) << totalPrice;
                    UIColors::printCentered("Total Price: RM " + priceStream.str(), SCREEN_WIDTH, UIColors::GREEN + UIColors::BOLD);
                    UIColors::printCentered("Rental Date: " + rentalDate + ", Duration: " + std::to_string(duration) + " days", SCREEN_WIDTH, UIColors::WHITE);
                    std::cout << std::endl;
                    
                    // Ask for payment method
                    UIColors::printCenteredInput("Payment Method (Cash/Credit Card/Debit Card/Online)*: ", SCREEN_WIDTH, UIColors::WHITE);
                    std::string paymentMethod;
                    std::getline(std::cin, paymentMethod);
                    
                    // Create rental
                    std::vector<int> dressIDs = {dressID};
                    int rentalID = rm.createRental(newCustomerID, rentalDate, duration, dressIDs);
                    
                    if (rentalID > 0) {
                        // Create payment
                        PaymentManager pm;
                        std::string paymentDate = rentalDate; // Use rental date as payment date
                        if (pm.createPayment(rentalID, totalPrice, paymentMethod, paymentDate, "")) {
                            InputValidator::showSuccess("Dress order and payment created successfully!");
                            UIColors::printCentered("Rental ID: " + std::to_string(rentalID), SCREEN_WIDTH, UIColors::CYAN);
                            UIColors::printCentered("Payment Method: " + paymentMethod, SCREEN_WIDTH, UIColors::CYAN);
                        } else {
                            InputValidator::showWarning("Rental created but payment processing failed. Please process payment manually.");
                        }
                    } else {
                        InputValidator::showError("Failed to create dress order.");
                    }
                    
                    delete selectedDress;
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
                UIColors::printCenteredInput("Enter search term (name, IC, phone, or email): ", SCREEN_WIDTH, UIColors::WHITE);
                searchTerm = InputValidator::getString("", false, 1, 100);
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
                
                UIColors::printCenteredInput("Enter Customer ID to update (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                customerID = InputValidator::getInt("", 0);
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
                UIColors::printCentered("Enter new values (press Enter to keep current value):", SCREEN_WIDTH, UIColors::WHITE);
                std::cout << std::endl;
                
                std::string input;
                
                std::string prompt1 = "Name [" + existing->Name + "]: ";
                UIColors::printCenteredInput(prompt1, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                customer.Name = input.empty() ? existing->Name : input;
                
                std::string prompt2 = "IC Number [" + existing->IC_Number + "]: ";
                UIColors::printCenteredInput(prompt2, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                customer.IC_Number = input.empty() ? existing->IC_Number : input;
                
                std::string prompt3 = "Phone [" + existing->Phone + "]: ";
                UIColors::printCenteredInput(prompt3, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                customer.Phone = input.empty() ? existing->Phone : input;
                
                std::string prompt4 = "Email [" + existing->Email + "]: ";
                UIColors::printCenteredInput(prompt4, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                customer.Email = input.empty() ? existing->Email : input;
                
                std::string prompt5 = "Address [" + existing->Address + "]: ";
                UIColors::printCenteredInput(prompt5, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                customer.Address = input.empty() ? existing->Address : input;
                
                std::string prompt6 = "Date of Birth [" + existing->DateOfBirth + "]: ";
                UIColors::printCenteredInput(prompt6, SCREEN_WIDTH, UIColors::YELLOW);
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
                
                UIColors::printCenteredInput("Enter Customer ID to delete (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                customerID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Customer ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                customerID = InputValidator::getInt("", 0);
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
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 7);
        
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
                
                std::string condPrompt = "Condition Status (Good/Fair/Poor) [Good]: ";
                UIColors::printCenteredInput(condPrompt, SCREEN_WIDTH, UIColors::YELLOW);
                std::string cond;
                std::getline(std::cin, cond);
                dress.ConditionStatus = cond.empty() ? "Good" : cond;
                
                std::string availPrompt = "Availability Status (Available/Rented/Maintenance) [Available]: ";
                UIColors::printCenteredInput(availPrompt, SCREEN_WIDTH, UIColors::YELLOW);
                std::string avail;
                std::getline(std::cin, avail);
                dress.AvailabilityStatus = avail.empty() ? "Available" : avail;
                
                std::string cleanPrompt = "Cleaning Status (Clean/Needs Cleaning) [Clean]: ";
                UIColors::printCenteredInput(cleanPrompt, SCREEN_WIDTH, UIColors::YELLOW);
                std::string clean;
                std::getline(std::cin, clean);
                dress.CleaningStatus = clean.empty() ? "Clean" : clean;
                
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
                UIColors::printCenteredInput("Enter search term: ", SCREEN_WIDTH, UIColors::WHITE);
                searchTerm = InputValidator::getString("", false, 1, 100);
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
                
                UIColors::printCenteredInput("Enter Dress ID to update (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                dressID = InputValidator::getInt("", 0);
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
                
                UIColors::printCentered("Current dress information:", SCREEN_WIDTH, UIColors::CYAN);
                dm.displayDress(*existing);
                UIColors::printCentered("Enter new values (press Enter to keep current value):", SCREEN_WIDTH, UIColors::WHITE);
                std::cout << std::endl;
                
                std::string input;
                
                std::string dPrompt1 = "Dress Name [" + existing->DressName + "]: ";
                UIColors::printCenteredInput(dPrompt1, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.DressName = input.empty() ? existing->DressName : input;
                
                std::string dPrompt2 = "Category [" + existing->Category + "]: ";
                UIColors::printCenteredInput(dPrompt2, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.Category = input.empty() ? existing->Category : input;
                
                std::string dPrompt3 = "Size [" + existing->Size + "]: ";
                UIColors::printCenteredInput(dPrompt3, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.Size = input.empty() ? existing->Size : input;
                
                std::string dPrompt4 = "Color [" + existing->Color + "]: ";
                UIColors::printCenteredInput(dPrompt4, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.Color = input.empty() ? existing->Color : input;
                
                std::string dPrompt5 = "Rental Price [" + std::to_string(existing->RentalPrice) + "]: ";
                UIColors::printCenteredInput(dPrompt5, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.RentalPrice = input.empty() ? existing->RentalPrice : std::stod(input);
                
                std::string dPrompt6 = "Condition Status [" + existing->ConditionStatus + "]: ";
                UIColors::printCenteredInput(dPrompt6, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.ConditionStatus = input.empty() ? existing->ConditionStatus : input;
                
                std::string availPrompt7 = "Availability Status [" + existing->AvailabilityStatus + "]: ";
                UIColors::printCenteredInput(availPrompt7, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.AvailabilityStatus = input.empty() ? existing->AvailabilityStatus : input;
                
                std::string cleanPrompt8 = "Cleaning Status [" + existing->CleaningStatus + "]: ";
                UIColors::printCenteredInput(cleanPrompt8, SCREEN_WIDTH, UIColors::YELLOW);
                std::getline(std::cin, input);
                dress.CleaningStatus = input.empty() ? existing->CleaningStatus : input;
                
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
                
                UIColors::printCenteredInput("Enter Dress ID to delete (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                dressID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Dress ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                dressID = InputValidator::getInt("", 0);
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
        UIColors::printMenuOption(7, "Change Rental Status");
        std::cout << std::endl;
        UIColors::printMenuOption(0, "Back to Main Menu");
        UIColors::printSeparator(SCREEN_WIDTH);
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 7);
        
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
                
                UIColors::printCenteredInput("Customer ID* (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                customerID = InputValidator::getInt("", 0);
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
                
                UIColors::printCentered("Enter dress IDs (1-5 dresses, enter 0 to finish):", SCREEN_WIDTH, UIColors::WHITE);
                std::cout << std::endl;
                int dressID;
                while (dressIDs.size() < 5) {
                    std::string prompt = "Dress ID " + std::to_string(dressIDs.size() + 1) + " (0 to finish): ";
                    UIColors::printCenteredInput(prompt, SCREEN_WIDTH, UIColors::WHITE);
                    dressID = InputValidator::getInt("", 0);
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
                
                UIColors::printCentered("Enter Rental ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                rentalID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Rental ID to return (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                rentalID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Customer ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                customerID = InputValidator::getInt("", 0);
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
            case 7: {
                // Show all rentals first
                UIColors::printHeader("CHANGE RENTAL STATUS", SCREEN_WIDTH);
                UIColors::printInfo("Please select a rental to change status:");
                
                std::vector<Rental> allRentals = rm.getActiveRentals();
                std::vector<Rental> returnedRentals;
                try {
                    sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
                        "SELECT * FROM Rentals WHERE Status = 'Returned' ORDER BY ReturnDate DESC"
                    );
                    while (res && res->next()) {
                        Rental rental;
                        rental.RentalID = res->getInt("RentalID");
                        rental.CustomerID = res->getInt("CustomerID");
                        rental.RentalDate = res->getString("RentalDate");
                        rental.DueDate = res->getString("DueDate");
                        rental.ReturnDate = res->getString("ReturnDate");
                        rental.TotalAmount = res->getDouble("TotalAmount");
                        if (res->wasNull()) rental.TotalAmount = 0.0;
                        rental.LateFee = res->getDouble("LateFee");
                        if (res->wasNull()) rental.LateFee = 0.0;
                        rental.Status = res->getString("Status");
                        returnedRentals.push_back(rental);
                    }
                    if (res) delete res;
                } catch (sql::SQLException& e) {
                    std::cerr << "Error getting returned rentals: " << e.what() << std::endl;
                }
                
                if (allRentals.empty() && returnedRentals.empty()) {
                    UIColors::printInfo("No rentals found.");
                    InputValidator::pause();
                    break;
                }
                
                if (!allRentals.empty()) {
                    UIColors::printCentered("=== ACTIVE RENTALS ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                    for (const auto& rental : allRentals) {
                        rm.displayRental(rental);
                    }
                }
                
                if (!returnedRentals.empty()) {
                    UIColors::printCentered("=== RETURNED RENTALS ===", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                    for (const auto& rental : returnedRentals) {
                        rm.displayRental(rental);
                    }
                }
                
                UIColors::printCenteredInput("Enter Rental ID to change status (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                rentalID = InputValidator::getInt("", 0);
                if (rentalID == 0) {
                    UIColors::printInfo("Status change cancelled.");
                    InputValidator::pause();
                    break;
                }
                
                Rental* rental = rm.getRentalByID(rentalID);
                if (!rental) {
                    InputValidator::showError("Rental not found.");
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printInfo("Current rental information:");
                rm.displayRental(*rental);
                
                std::cout << std::endl;
                UIColors::printCentered("Available Status Options:", SCREEN_WIDTH, UIColors::YELLOW);
                UIColors::printCentered("1. Active (Mark as Active/Rented)", SCREEN_WIDTH, UIColors::WHITE);
                UIColors::printCentered("2. Returned (Mark as Returned)", SCREEN_WIDTH, UIColors::WHITE);
                std::cout << std::endl;
                
                UIColors::printCenteredInput("Select new status (1=Active, 2=Returned, 0=Cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                int statusChoice = InputValidator::getInt("", 0, 2);
                
                if (statusChoice == 0) {
                    UIColors::printInfo("Status change cancelled.");
                    delete rental;
                    InputValidator::pause();
                    break;
                }
                
                std::string newStatus = (statusChoice == 1) ? "Active" : "Returned";
                std::string newReturnDate = "";
                
                if (statusChoice == 2 && rental->Status != "Returned") {
                    // If changing to Returned, ask for return date
                    newReturnDate = InputValidator::getDate("Return Date*");
                } else if (statusChoice == 2) {
                    // Already returned, use existing return date
                    newReturnDate = rental->ReturnDate;
                }
                
                if (InputValidator::confirm("Are you sure you want to change this rental status to '" + newStatus + "'?")) {
                    if (rm.updateRentalStatus(rentalID, newStatus, newReturnDate)) {
                        InputValidator::showSuccess("Rental status updated successfully!");
                    } else {
                        InputValidator::showError("Failed to update rental status.");
                    }
                } else {
                    UIColors::printInfo("Status change cancelled.");
                }
                
                delete rental;
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
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 5);
        
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
                
                UIColors::printCenteredInput("Rental ID* (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                rentalID = InputValidator::getInt("", 0);
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
                
                UIColors::printCentered("Payment Summary", SCREEN_WIDTH, UIColors::BOLD + UIColors::CYAN);
                UIColors::printCentered("Total Due: RM " + std::to_string(totalDue), SCREEN_WIDTH, UIColors::RED);
                UIColors::printCentered("Total Paid: RM " + std::to_string(totalPaid), SCREEN_WIDTH, UIColors::GREEN);
                UIColors::printCentered("Remaining: RM " + std::to_string(remaining), SCREEN_WIDTH, UIColors::YELLOW);
                
                if (remaining <= 0) {
                    UIColors::printInfo("This rental is already fully paid.");
                    delete rental;
                    InputValidator::pause();
                    break;
                }
                
                UIColors::printCenteredInput("Payment Amount (RM)*: ", SCREEN_WIDTH, UIColors::WHITE);
                amount = InputValidator::getDouble("", 0.01, remaining);
                UIColors::printCenteredInput("Payment Method (Cash/Credit Card/Debit Card/Online)*: ", SCREEN_WIDTH, UIColors::WHITE);
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
                
                UIColors::printCentered("Enter Rental ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                rentalID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Payment ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                paymentID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("Enter Payment ID (0 to cancel): ", SCREEN_WIDTH, UIColors::WHITE);
                paymentID = InputValidator::getInt("", 0);
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
                
                UIColors::printCenteredInput("New Status (Pending/Completed/Failed/Refunded)*: ", SCREEN_WIDTH, UIColors::WHITE);
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
    if (!auth.hasPermission("Administrator")) {
        UIColors::printError("Access denied. Administrator role required to view Reports & Analytics.");
        UIColors::printCentered("Please contact your administrator for access.", SCREEN_WIDTH, UIColors::YELLOW);
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
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 9);
        
        if (choice == 0) break;
        
        std::string year, startDate, endDate;
        
        switch (choice) {
            case 1:
                UIColors::printCenteredInput("Enter year (YYYY): ", SCREEN_WIDTH, UIColors::WHITE);
                year = InputValidator::getString("", true, 4, 4);
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
        
        UIColors::printCenteredInput("Enter your choice: ", SCREEN_WIDTH, UIColors::WHITE);
        choice = InputValidator::getInt("", 0, 4);
        if (choice == 0) break;
        
        // User management implementation here
        UIColors::printInfo("User management features - to be implemented");
        InputValidator::pause();
    }
}


