#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <string>
#include <iostream>
#include <sstream>
#include <regex>
#include <limits>

class InputValidator {
public:
    // Integer input with validation
    static int getInt(const std::string& prompt, int min = INT_MIN, int max = INT_MAX, bool allowRetry = true);
    
    // Double input with validation
    static double getDouble(const std::string& prompt, double min = -std::numeric_limits<double>::max(), 
                           double max = std::numeric_limits<double>::max(), bool allowRetry = true);
    
    // String input with validation
    static std::string getString(const std::string& prompt, bool required = true, 
                                 size_t minLength = 0, size_t maxLength = 255);
    
    // Date input with format validation (YYYY-MM-DD)
    static std::string getDate(const std::string& prompt, bool allowRetry = true);
    
    // Email validation
    static bool isValidEmail(const std::string& email);
    
    // Phone validation
    static bool isValidPhone(const std::string& phone);
    
    // IC number validation (basic)
    static bool isValidIC(const std::string& ic);
    
    // Confirmation prompt
    static bool confirm(const std::string& message);
    
    // Clear input buffer
    static void clearInputBuffer();
    
    // Pause for user
    static void pause(const std::string& message = "Press Enter to continue...");
    
    // Display error message
    static void showError(const std::string& message);
    
    // Display success message
    static void showSuccess(const std::string& message);
    
    // Display info message
    static void showInfo(const std::string& message);
    
    // Display warning message
    static void showWarning(const std::string& message);
    
    // Validate date format
    static bool isValidDate(const std::string& date);
    
    // Validate date range
    static bool isDateInRange(const std::string& date, const std::string& minDate = "", 
                              const std::string& maxDate = "");
};

#endif

