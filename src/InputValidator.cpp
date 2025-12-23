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

void InputValidator::printTip(const std::string& tip) {
    UIColors::printInfo("Tip: " + tip);
}

std::string InputValidator::formatRangeTip(int min, int max) {
    std::ostringstream oss;
    if (min != INT_MIN && max != INT_MAX) {
        oss << "Enter a value between " << min << " and " << max << ".";
    } else if (min != INT_MIN) {
        oss << "Enter a value of at least " << min << ".";
    } else if (max != INT_MAX) {
        oss << "Enter a value no more than " << max << ".";
    } else {
        oss << "Enter a whole number without letters or symbols.";
    }
    return oss.str();
}

std::string InputValidator::formatRangeTip(double min, double max) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    if (min != (std::numeric_limits<double>::lowest()) && max != (std::numeric_limits<double>::max)()) {
        oss << "Enter a value between " << min << " and " << max << ".";
    } else if (min != (std::numeric_limits<double>::lowest())) {
        oss << "Enter a value of at least " << min << ".";
    } else if (max != (std::numeric_limits<double>::max)()) {
        oss << "Enter a value no more than " << max << ".";
    } else {
        oss << "Enter a numeric value without commas.";
    }
    return oss.str();
}

int InputValidator::getInt(const std::string& prompt, int min, int max, bool allowRetry) {
    int value;
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCenteredInput(prompt, SCREEN_WIDTH, UIColors::WHITE);
        }
        // Don't add padding for empty prompts - let the caller handle positioning
        std::getline(std::cin, input);
        
        if (input.empty()) {
            UIColors::printCentered("Input cannot be empty. Please enter a number.", SCREEN_WIDTH, UIColors::RED);
            printTip(formatRangeTip(min, max));
            if (!allowRetry) return 0;
            continue;
        }
        
        try {
            value = std::stoi(input);
            
            if (value < min || value > max) {
                UIColors::printCentered("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".", SCREEN_WIDTH, UIColors::RED);
                printTip(formatRangeTip(min, max));
                if (!allowRetry) return 0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            UIColors::printCentered("Invalid input. Please enter a valid number.", SCREEN_WIDTH, UIColors::RED);
            printTip("Use digits only (no spaces or letters).");
            if (!allowRetry) return 0;
        }
    }
}

double InputValidator::getDouble(const std::string& prompt, double min, double max, bool allowRetry) {
    double value;
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCenteredInput(prompt, SCREEN_WIDTH, UIColors::WHITE);
        }
        // Don't add padding for empty prompts - let the caller handle positioning
        std::getline(std::cin, input);
        
        if (input.empty()) {
            UIColors::printCentered("Input cannot be empty. Please enter a number.", SCREEN_WIDTH, UIColors::RED);
            printTip(formatRangeTip(min, max));
            if (!allowRetry) return 0.0;
            continue;
        }
        
        try {
            value = std::stod(input);
            
            if (value < min || value > max) {
                UIColors::printCentered("Value must be between " + std::to_string(min) + " and " + std::to_string(max) + ".", SCREEN_WIDTH, UIColors::RED);
                printTip(formatRangeTip(min, max));
                if (!allowRetry) return 0.0;
                continue;
            }
            
            return value;
        } catch (const std::exception&) {
            UIColors::printCentered("Invalid input. Please enter a valid number.", SCREEN_WIDTH, UIColors::RED);
            printTip("Enter a numeric value such as 10.00 (avoid letters and commas).");
            if (!allowRetry) return 0.0;
        }
    }
}

std::string InputValidator::getString(const std::string& prompt, bool required, size_t minLength, size_t maxLength) {
    std::string input;
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCenteredInput(prompt, SCREEN_WIDTH, UIColors::WHITE);
        }
        // Don't add padding for empty prompts - let the caller handle positioning
        std::getline(std::cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r"));
        input.erase(input.find_last_not_of(" \t\n\r") + 1);
        
        if (required && input.empty()) {
            UIColors::printCentered("This field is required and cannot be empty.", SCREEN_WIDTH, UIColors::RED);
            printTip("Type something meaningful; avoid leaving this blank.");
            continue;
        }
        
        if (input.length() < minLength) {
            UIColors::printCentered("Input must be at least " + std::to_string(minLength) + " characters long.", SCREEN_WIDTH, UIColors::RED);
            printTip("Add a few more characters so it meets the minimum length.");
            continue;
        }
        
        if (input.length() > maxLength) {
            UIColors::printCentered("Input must not exceed " + std::to_string(maxLength) + " characters.", SCREEN_WIDTH, UIColors::RED);
            printTip("Keep it concise; remove extra spaces or words.");
            continue;
        }
        
        return input;
    }
}

std::string InputValidator::getDate(const std::string& prompt, bool allowRetry) {
    std::string input;
    
    while (true) {
        std::string fullPrompt = prompt + " (YYYY-MM-DD)";
        UIColors::printCenteredInput(fullPrompt, SCREEN_WIDTH, UIColors::WHITE);
        std::getline(std::cin, input);
        
        if (input.empty()) {
            showError("Date cannot be empty.");
            printTip("Enter the date in YYYY-MM-DD format, e.g., 2025-01-15.");
            if (!allowRetry) return "";
            continue;
        }
        
        if (!isValidDate(input)) {
            showError("Invalid date format. Please use YYYY-MM-DD format (e.g., 2000-01-15).");
            printTip("Double-check the month/day and include leading zeros (04 for April).");
            if (!allowRetry) return "";
            continue;
        }
        
        return input;
    }
}

bool InputValidator::isValidEmail(const std::string& email, std::string* tipOut) {
    if (email.empty()) {
        if (tipOut) *tipOut = "Email cannot be empty when required.";
        return false;
    }
    
    std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    bool valid = std::regex_match(email, pattern);
    if (!valid) {
        if (tipOut) {
            *tipOut = "Use a format like user@example.com (letters/numbers only).";
        } else {
            printTip("Use a format like user@example.com (letters/numbers only).");
        }
    }
    return valid;
}

bool InputValidator::isValidPhone(const std::string& phone, std::string* tipOut) {
    if (phone.empty()) {
        if (tipOut) *tipOut = "Phone number cannot be empty when required.";
        return false;
    }
    
    // Allow digits, spaces, hyphens, parentheses, and +
    std::regex pattern(R"([+]?[\d\s\-\(\)]{8,20})");
    bool valid = std::regex_match(phone, pattern);
    if (!valid) {
        if (tipOut) {
            *tipOut = "Keep 8-20 characters; digits only with optional +, spaces, or dashes.";
        } else {
            printTip("Use 8-20 digits; you may include +, spaces, or dashes.");
        }
    }
    return valid;
}

bool InputValidator::isValidIC(const std::string& ic, std::string* tipOut) {
    if (ic.empty()) {
        if (tipOut) *tipOut = "IC number cannot be empty when required.";
        return false;
    }
    
    // Basic validation: alphanumeric, 8-20 characters
    std::regex pattern(R"([A-Za-z0-9]{8,20})");
    bool valid = std::regex_match(ic, pattern);
    if (!valid) {
        if (tipOut) {
            *tipOut = "Use 8-20 letters or numbers with no spaces.";
        } else {
            printTip("Use 8-20 letters or numbers with no spaces.");
        }
    }
    return valid;
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

std::string InputValidator::getPassword(const std::string& prompt, bool showRequirements, bool skipValidation) {
    std::string password;
    
    if (showRequirements) {
        std::cout << std::endl;
        UIColors::printCentered("Password Requirements:", SCREEN_WIDTH, UIColors::YELLOW);
        UIColors::printCentered("- At least 8 characters long", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("- At least one uppercase letter (A-Z)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("- At least one lowercase letter (a-z)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("- At least one digit (0-9)", SCREEN_WIDTH, UIColors::WHITE);
        UIColors::printCentered("- At least one special character (!@#$%^&*()_+-=[]{}|;:,.<>?)", SCREEN_WIDTH, UIColors::WHITE);
        std::cout << std::endl;
    }
    
    while (true) {
        if (!prompt.empty()) {
            UIColors::printCenteredInput(prompt, SCREEN_WIDTH, UIColors::WHITE);
        }
        // Don't add padding for empty prompts - let the caller handle positioning
        
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
            printTip("Type your password, then press Enter.");
            std::cout << std::endl;
            continue;
        }
        
        // Skip validation for login (existing passwords may not meet new requirements)
        if (skipValidation) {
            return password;
        }
        
        std::string errorMsg;
        if (isValidPassword(password, errorMsg)) {
            return password;
        } else {
            UIColors::printCentered(errorMsg, SCREEN_WIDTH, UIColors::RED);
            printTip("Try mixing uppercase, lowercase, digits, and symbols.");
            std::cout << std::endl;
            continue;
        }
    }
}

bool InputValidator::confirm(const std::string& message) {
    std::string input;
    std::cout << std::endl;
    UIColors::printCentered(message + " (y/n): ", SCREEN_WIDTH, UIColors::YELLOW);
    std::getline(std::cin, input);
    
    // Convert to lowercase
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    if (input.empty()) {
        return false;
    }
    
    if (input == "y" || input == "yes") return true;
    if (input == "n" || input == "no") return false;
    
    UIColors::printCentered("Please respond with 'y' or 'n'.", SCREEN_WIDTH, UIColors::RED);
    printTip("Type y for yes or n for no, then press Enter.");
    return false;
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

