#ifndef UICOLORS_H
#define UICOLORS_H

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>

class UIColors {
public:
    // ANSI Color Codes
    static const std::string RESET;
    static const std::string BOLD;
    static const std::string DIM;
    
    // Text Colors
    static const std::string BLACK;
    static const std::string RED;
    static const std::string GREEN;
    static const std::string YELLOW;
    static const std::string BLUE;
    static const std::string MAGENTA;
    static const std::string CYAN;
    static const std::string WHITE;
    
    // Background Colors
    static const std::string BG_BLACK;
    static const std::string BG_RED;
    static const std::string BG_GREEN;
    static const std::string BG_YELLOW;
    static const std::string BG_BLUE;
    static const std::string BG_MAGENTA;
    static const std::string BG_CYAN;
    static const std::string BG_WHITE;
    
    // Utility Functions
    static void enableColors();
    static void disableColors();
    static bool colorsEnabled();
    
    // Centered Text Functions
    static void printCentered(const std::string& text, int width = 80, const std::string& color = "");
    static void printHeader(const std::string& title, int width = 80);
    static void printSeparator(int width = 80, char ch = '=');
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printInfo(const std::string& message);
    static void printMenuOption(int num, const std::string& text);
    
    // Colored Text Functions
    static std::string colorize(const std::string& text, const std::string& color);
    static std::string bold(const std::string& text);
    
private:
    static bool colors_enabled;
};

#endif

