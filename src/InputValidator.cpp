#include "InputValidator.h"
#include "UIColors.h"
#include "MenuHandlers.h"  // For SCREEN_WIDTH
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cctype>
#ifdef _WIN32
#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

int InputValidator::getInt(const std::string& prompt, int min, int max, bool allowRetry) {
    int value;
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCentered(prompt, SCREEN_WIDTH, UIColors::WHITE);
            std::cout << "  ";
        } else {
            std::cout << prompt;
        }
        std::getline(std::cin, input);
        
        if (input.empty()) {
            UIColors::printCentered("Input cannot be empty. Please enter a number.", SCREEN_WIDTH, UIColors::RED);
            if (!allowRetry) return 0;
            continue;
        }
        
        try {
            value = std::stoi(input);
            
            if (value < min || value > max) {
                UIColors::printCentered("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".", SCREEN_WIDTH, UIColors::RED);
                if (!allowRetry) return 0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            UIColors::printCentered("Invalid input. Please enter a valid number.", SCREEN_WIDTH, UIColors::RED);
            if (!allowRetry) return 0;
        }
    }
}

double InputValidator::getDouble(const std::string& prompt, double min, double max, bool allowRetry) {
    double value;
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCentered(prompt, SCREEN_WIDTH, UIColors::WHITE);
            std::cout << "  ";
        } else {
            std::cout << prompt;
        }
        std::getline(std::cin, input);
        
        if (input.empty()) {
            UIColors::printCentered("Input cannot be empty. Please enter a number.", SCREEN_WIDTH, UIColors::RED);
            if (!allowRetry) return 0.0;
            continue;
        }
        
        try {
            value = std::stod(input);
            
            if (value < min || value > max) {
                UIColors::printCentered("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".", SCREEN_WIDTH, UIColors::RED);
                if (!allowRetry) return 0.0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            UIColors::printCentered("Invalid input. Please enter a valid number.", SCREEN_WIDTH, UIColors::RED);
            if (!allowRetry) return 0.0;
        }
    }
}

std::string InputValidator::getString(const std::string& prompt, bool required, size_t minLength, size_t maxLength) {
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCentered(prompt, SCREEN_WIDTH, UIColors::WHITE);
            std::cout << "  ";
        } else {
            std::cout << prompt;
        }
        std::getline(std::cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        
        if (required && input.empty()) {
            UIColors::printCentered("This field is required and cannot be empty.", SCREEN_WIDTH, UIColors::RED);
            continue;
        }
        
        if (input.length() < minLength) {
            UIColors::printCentered("Input must be at least " + std::to_string(minLength) + " characters long.", SCREEN_WIDTH, UIColors::RED);
            continue;
        }
        
        if (input.length() > maxLength) {
            UIColors::printCentered("Input must not exceed " + std::to_string(maxLength) + " characters.", SCREEN_WIDTH, UIColors::RED);
            continue;
        }
        
        return input;
    }
}

std::string InputValidator::getDate(const std::string& prompt, bool allowRetry) {
    std::string input;
    
    while (true) {
        std::cout << prompt << " (YYYY-MM-DD): ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            showError("Date cannot be empty.");
            if (!allowRetry) return "";
            continue;
        }
        
        if (!isValidDate(input)) {
            showError("Invalid date format. Please use YYYY-MM-DD format (e.g., 2000-01-15).");
            if (!allowRetry) return "";
            continue;
        }
        
        return input;
    }
}

bool InputValidator::isValidEmail(const std::string& email) {
    if (email.empty()) return false;
    
    std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool InputValidator::isValidPhone(const std::string& phone) {
    if (phone.empty()) return false;
    
    // Allow digits, spaces, hyphens, parentheses, and +
    std::regex pattern(R"([+]?[\d\s\-\(\)]{8,20})");
    return std::regex_match(phone, pattern);
}

bool InputValidator::isValidIC(const std::string& ic) {
    if (ic.empty()) return false;
    
    // Basic validation: alphanumeric, 8-20 characters
    std::regex pattern(R"([A-Za-z0-9]{8,20})");
    return std::regex_match(ic, pattern);
}

bool InputValidator::isValidPassword(const std::string& password, std::string& errorMessage) {
    errorMessage = "";
    
    if (password.length() < 8) {
        errorMessage = "Password must be at least 8 characters long.";
        return false;
    }
    
    if (password.length() > 50) {
        errorMessage = "Password must not exceed 50 characters.";
        return false;
    }
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else if (std::ispunct(c)) hasSpecial = true;
    }
    
    if (!hasUpper) {
        errorMessage = "Password must contain at least one uppercase letter (A-Z).";
        return false;
    }
    
    if (!hasLower) {
        errorMessage = "Password must contain at least one lowercase letter (a-z).";
        return false;
    }
    
    if (!hasDigit) {
        errorMessage = "Password must contain at least one digit (0-9).";
        return false;
    }
    
    if (!hasSpecial) {
        errorMessage = "Password must contain at least one special character (!@#$%^&*()_+-=[]{}|;:,.<>?).";
        return false;
    }
    
    return true;
}

std::string InputValidator::getPassword(const std::string& prompt, bool showRequirements) {
    std::string password;
    
    if (showRequirements) {
        std::cout << std::endl;
        UIColors::printCentered("Password Requirements:", SCREEN_WIDTH, UIColors::YELLOW);
        UIColors::printCentered("• At least 8 characters long", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("• At least one uppercase letter (A-Z)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("• At least one lowercase letter (a-z)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("• At least one digit (0-9)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("• At least one special character (!@#$%^&*()_+-=[]{}|;:,.<>?)", SCREEN_WIDTH, UIColors::WHITE);
        std::cout << std::endl;
    }
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCentered(prompt, SCREEN_WIDTH, UIColors::WHITE);
        }
        std::cout << "  ";
        
        // Mask password input
        password = "";
        char ch;
#ifdef _WIN32
        // Windows implementation
        while ((ch = _getch()) != '\r' && ch != '\n') {
            if (ch == '\b') {  // Backspace
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch >= 32 && ch < 127) {  // Printable characters
                password += ch;
                std::cout << '*';
            }
        }
        std::cout << std::endl;
#else
        // Linux/Unix implementation
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        std::getline(std::cin, password);
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        std::cout << std::endl;
#endif
        
        // Trim whitespace
        password.erase(0, password.find_first_not_of(" \t\n\r"));
        password.erase(password.find_last_not_of(" \t\n\r") + 1);
        
        if (password.empty()) {
            UIColors::printCentered("Password cannot be empty.", SCREEN_WIDTH, UIColors::RED);
            std::cout << std::endl;
            continue;
        }
        
        std::string errorMsg;
        if (isValidPassword(password, errorMsg)) {
            return password;
        } else {
            UIColors::printCentered(errorMsg, SCREEN_WIDTH, UIColors::RED);
            std::cout << std::endl;
            continue;
        }
    }
}

bool InputValidator::confirm(const std::string& message) {
    std::string input;
    std::cout << std::endl;
    UIColors::printCentered(message + " (y/n): ", SCREEN_WIDTH, UIColors::YELLOW);
    std::cout << "  ";
    std::getline(std::cin, input);
    
    // Convert to lowercase
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    return input == "y" || input == "yes";
}

void InputValidator::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
}

void InputValidator::pause(const std::string& message) {
    std::cout << std::endl;
    UIColors::printCentered(message, SCREEN_WIDTH, UIColors::CYAN);
    std::cin.get();
}

void InputValidator::showError(const std::string& message) {
    UIColors::printError(message);
}

void InputValidator::showSuccess(const std::string& message) {
    UIColors::printSuccess(message);
}

void InputValidator::showInfo(const std::string& message) {
    UIColors::printInfo(message);
}

void InputValidator::showWarning(const std::string& message) {
    UIColors::printWarning(message);
}

bool InputValidator::isValidDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));
        
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        
        // Basic validation - check for obvious invalid dates
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
        if (month == 2 && day > 29) return false;
        
        // Validate using tm structure
        std::tm tm = {};
        std::istringstream ss(date);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        return !ss.fail();
    } catch (const std::exception&) {
        return false;
    }
}

bool InputValidator::isDateInRange(const std::string& date, const std::string& minDate, const std::string& maxDate) {
    if (!isValidDate(date)) return false;
    
    if (!minDate.empty() && date < minDate) return false;
    if (!maxDate.empty() && date > maxDate) return false;
    
    return true;
}

