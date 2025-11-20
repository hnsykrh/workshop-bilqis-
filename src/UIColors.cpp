#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include <windows.h>
#endif

bool UIColors::colors_enabled = true;

const std::string UIColors::RESET = "\033[0m";
const std::string UIColors::BOLD = "\033[1m";
const std::string UIColors::DIM = "\033[2m";

const std::string UIColors::BLACK = "\033[30m";
const std::string UIColors::RED = "\033[31m";
const std::string UIColors::GREEN = "\033[32m";
const std::string UIColors::YELLOW = "\033[33m";
const std::string UIColors::BLUE = "\033[34m";
const std::string UIColors::MAGENTA = "\033[35m";
const std::string UIColors::CYAN = "\033[36m";
const std::string UIColors::WHITE = "\033[37m";

const std::string UIColors::BG_BLACK = "\033[40m";
const std::string UIColors::BG_RED = "\033[41m";
const std::string UIColors::BG_GREEN = "\033[42m";
const std::string UIColors::BG_YELLOW = "\033[43m";
const std::string UIColors::BG_BLUE = "\033[44m";
const std::string UIColors::BG_MAGENTA = "\033[45m";
const std::string UIColors::BG_CYAN = "\033[46m";
const std::string UIColors::BG_WHITE = "\033[47m";

void UIColors::enableColors() {
    colors_enabled = true;
#ifdef _WIN32
    // Enable ANSI escape sequences in Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void UIColors::disableColors() {
    colors_enabled = false;
}

bool UIColors::colorsEnabled() {
    return colors_enabled;
}

void UIColors::printCentered(const std::string& text, int width, const std::string& color) {
    // Handle long text by splitting into multiple lines
    if (text.length() > static_cast<size_t>(width - 4)) {
        // Split long text
        std::istringstream iss(text);
        std::string word;
        std::string line;
        
        while (iss >> word) {
            if ((line + word).length() < static_cast<size_t>(width - 4)) {
                if (!line.empty()) line += " ";
                line += word;
            } else {
                if (!line.empty()) {
                    int padding = (width - static_cast<int>(line.length())) / 2;
                    if (padding < 0) padding = 0;
                    std::string output = std::string(padding, ' ') + line;
                    if (colors_enabled && !color.empty()) {
                        std::cout << color << output << RESET << std::endl;
                    } else {
                        std::cout << output << std::endl;
                    }
                }
                line = word;
            }
        }
        if (!line.empty()) {
            int padding = (width - static_cast<int>(line.length())) / 2;
            if (padding < 0) padding = 0;
            std::string output = std::string(padding, ' ') + line;
            if (colors_enabled && !color.empty()) {
                std::cout << color << output << RESET << std::endl;
            } else {
                std::cout << output << std::endl;
            }
        }
    } else {
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding < 0) padding = 0;
    
    std::string output = std::string(padding, ' ') + text;
    if (colors_enabled && !color.empty()) {
        std::cout << color << output << RESET << std::endl;
    } else {
        std::cout << output << std::endl;
        }
    }
}

void UIColors::printHeader(const std::string& title, int width) {
    printSeparator(width);
    printCentered(title, width, BOLD + CYAN);
    printSeparator(width);
}

void UIColors::printSeparator(int width, char ch) {
    std::string sep(width, ch);
    if (colors_enabled) {
        std::cout << CYAN << sep << RESET << std::endl;
    } else {
        std::cout << sep << std::endl;
    }
}

void UIColors::printSuccess(const std::string& message) {
    std::string fullMessage = "[SUCCESS] " + message;
    if (colors_enabled) {
        printCentered(fullMessage, 80, GREEN);
    } else {
        printCentered(fullMessage, 80);
    }
}

void UIColors::printError(const std::string& message) {
    std::string fullMessage = "[ERROR] " + message;
    if (colors_enabled) {
        printCentered(fullMessage, 80, RED);
    } else {
        printCentered(fullMessage, 80);
    }
}

void UIColors::printWarning(const std::string& message) {
    std::string fullMessage = "[WARNING] " + message;
    if (colors_enabled) {
        printCentered(fullMessage, 80, YELLOW);
    } else {
        printCentered(fullMessage, 80);
    }
}

void UIColors::printInfo(const std::string& message) {
    std::string fullMessage = "[INFO] " + message;
    if (colors_enabled) {
        printCentered(fullMessage, 80, BLUE);
    } else {
        printCentered(fullMessage, 80);
    }
}

void UIColors::printMenuOption(int num, const std::string& text) {
    std::ostringstream oss;
    oss << num << ". " << text;
    if (colors_enabled) {
        int padding = (SCREEN_WIDTH - static_cast<int>(text.length()) - 6) / 2;
        if (padding < 0) padding = 0;
        std::string output = std::string(padding, ' ') + std::to_string(num) + ". " + text;
        std::cout << std::string(padding, ' ') << CYAN << std::to_string(num) << RESET << ". " << WHITE << text << RESET << std::endl;
    } else {
        int padding = (SCREEN_WIDTH - static_cast<int>(text.length()) - 6) / 2;
        if (padding < 0) padding = 0;
        std::cout << std::string(padding, ' ') << std::setw(3) << num << ". " << text << std::endl;
    }
}

std::string UIColors::colorize(const std::string& text, const std::string& color) {
    if (colors_enabled) {
        return color + text + RESET;
    }
    return text;
}

std::string UIColors::bold(const std::string& text) {
    if (colors_enabled) {
        return BOLD + text + RESET;
    }
    return text;
}

