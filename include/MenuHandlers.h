#ifndef MENU_HANDLERS_H
#define MENU_HANDLERS_H

#include <string>

const int SCREEN_WIDTH = 80;

// Login and authentication
bool showLoginScreen();
void showChangePasswordMenu();
void displayMainMenu();

// Management menus
void customerManagementMenu();
void dressManagementMenu();
void rentalManagementMenu();
void paymentManagementMenu();
void reportsMenu();
void userManagementMenu();
void customerViewMenu();

#endif

