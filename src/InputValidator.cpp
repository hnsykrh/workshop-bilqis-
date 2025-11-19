#include "InputValidator.h"
#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <limits>

int InputValidator::getInt(const std::string& prompt, int min, int max, bool allowRetry) {
    int value;
    std::string input;
    
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            showError("Input cannot be empty. Please enter a number.");
            if (!allowRetry) return 0;
            continue;
        }
        
        try {
            value = std::stoi(input);
            
            if (value < min || value > max) {
                showError("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".");
                if (!allowRetry) return 0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            showError("Invalid input. Please enter a valid number.");
            if (!allowRetry) return 0;
        }
    }
}

double InputValidator::getDouble(const std::string& prompt, double min, double max, bool allowRetry) {
    double value;
    std::string input;
    
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            showError("Input cannot be empty. Please enter a number.");
            if (!allowRetry) return 0.0;
            continue;
        }
        
        try {
            value = std::stod(input);
            
            if (value < min || value > max) {
                showError("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".");
                if (!allowRetry) return 0.0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            showError("Invalid input. Please enter a valid number.");
            if (!allowRetry) return 0.0;
        }
    }
}

std::string InputValidator::getString(const std::string& prompt, bool required, size_t minLength, size_t maxLength) {
    std::string input;
    
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        
        if (required && input.empty()) {
            showError("This field is required and cannot be empty.");
            continue;
        }
        
        if (input.length() < minLength) {
            showError("Input must be at least " + std::to_string(minLength) + " characters long.");
            continue;
        }
        
        if (input.length() > maxLength) {
            showError("Input must not exceed " + std::to_string(maxLength) + " characters.");
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

bool InputValidator::confirm(const std::string& message) {
    std::string input;
    std::cout << "\n" << message << " (y/n): ";
    std::getline(std::cin, input);
    
    // Convert to lowercase
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    return input == "y" || input == "yes";
}

void InputValidator::clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void InputValidator::pause(const std::string& message) {
    std::cout << "\n" << message;
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

