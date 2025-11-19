#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
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
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding < 0) padding = 0;
    
    std::string output = std::string(padding, ' ') + text;
    if (colors_enabled && !color.empty()) {
        std::cout << color << output << RESET << std::endl;
    } else {
        std::cout << output << std::endl;
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
    if (colors_enabled) {
        std::cout << GREEN << "[SUCCESS] " << message << RESET << std::endl;
    } else {
        std::cout << "[SUCCESS] " << message << std::endl;
    }
}

void UIColors::printError(const std::string& message) {
    if (colors_enabled) {
        std::cout << RED << "[ERROR] " << message << RESET << std::endl;
    } else {
        std::cout << "[ERROR] " << message << std::endl;
    }
}

void UIColors::printWarning(const std::string& message) {
    if (colors_enabled) {
        std::cout << YELLOW << "[WARNING] " << message << RESET << std::endl;
    } else {
        std::cout << "[WARNING] " << message << std::endl;
    }
}

void UIColors::printInfo(const std::string& message) {
    if (colors_enabled) {
        std::cout << BLUE << "[INFO] " << message << RESET << std::endl;
    } else {
        std::cout << "[INFO] " << message << std::endl;
    }
}

void UIColors::printMenuOption(int num, const std::string& text) {
    std::ostringstream oss;
    oss << num << ". " << text;
    if (colors_enabled) {
        std::cout << YELLOW << std::setw(3) << num << RESET << ". " << WHITE << text << RESET << std::endl;
    } else {
        std::cout << std::setw(3) << num << ". " << text << std::endl;
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

