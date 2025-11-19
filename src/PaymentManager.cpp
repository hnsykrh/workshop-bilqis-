#include "PaymentManager.h"
#include "UIColors.h"
#include <iostream>
#include <iomanip>
#include <ctime>

bool PaymentManager::validatePaymentMethod(const std::string& method) {
    return method == "Cash" || method == "Credit Card" || 
           method == "Debit Card" || method == "Online";
}

bool PaymentManager::createPayment(int rentalID, double amount, const std::string& paymentMethod,
                                  const std::string& paymentDate, const std::string& transactionRef) {
    if (!validatePaymentMethod(paymentMethod)) {
        std::cerr << "Error: Invalid payment method. Use: Cash, Credit Card, Debit Card, or Online." << std::endl;
        return false;
    }
    
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) "
            "VALUES (?, ?, ?, ?, 'Completed', ?)"
        );
        
        pstmt->setInt(1, rentalID);
        pstmt->setDouble(2, amount);
        pstmt->setString(3, paymentMethod);
        pstmt->setString(4, paymentDate);
        pstmt->setString(5, transactionRef);
        
        pstmt->executeUpdate();
        delete pstmt;
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "Error creating payment: " << e.what() << std::endl;
        std::cerr << "MySQL Error Code: " << e.getErrorCode() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

Payment* PaymentManager::getPaymentByID(int paymentID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return nullptr;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Payments WHERE PaymentID = ?"
        );
        pstmt->setInt(1, paymentID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        if (res && res->next()) {
            Payment* payment = new Payment();
            payment->PaymentID = res->getInt("PaymentID");
            payment->RentalID = res->getInt("RentalID");
            payment->Amount = res->getDouble("Amount");
            payment->PaymentMethod = res->getString("PaymentMethod");
            payment->PaymentDate = res->getString("PaymentDate");
            payment->Status = res->getString("Status");
            payment->TransactionReference = res->getString("TransactionReference");
            delete pstmt;
            delete res;
            return payment;
        }
        delete pstmt;
        if (res) delete res;
        return nullptr;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting payment: " << e.what() << std::endl;
        return nullptr;
    }
}

std::vector<Payment> PaymentManager::getPaymentsByRental(int rentalID) {
    std::vector<Payment> payments;
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return payments;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT * FROM Payments WHERE RentalID = ? ORDER BY PaymentDate DESC"
        );
        pstmt->setInt(1, rentalID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        while (res && res->next()) {
            Payment payment;
            payment.PaymentID = res->getInt("PaymentID");
            payment.RentalID = res->getInt("RentalID");
            payment.Amount = res->getDouble("Amount");
            payment.PaymentMethod = res->getString("PaymentMethod");
            payment.PaymentDate = res->getString("PaymentDate");
            payment.Status = res->getString("Status");
            payment.TransactionReference = res->getString("TransactionReference");
            payments.push_back(payment);
        }
        delete pstmt;
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting payments: " << e.what() << std::endl;
    }
    return payments;
}

std::vector<Payment> PaymentManager::getAllPayments() {
    std::vector<Payment> payments;
    try {
        sql::ResultSet* res = DatabaseManager::getInstance().executeSelect(
            "SELECT * FROM Payments ORDER BY PaymentDate DESC"
        );
        
        while (res && res->next()) {
            Payment payment;
            payment.PaymentID = res->getInt("PaymentID");
            payment.RentalID = res->getInt("RentalID");
            payment.Amount = res->getDouble("Amount");
            payment.PaymentMethod = res->getString("PaymentMethod");
            payment.PaymentDate = res->getString("PaymentDate");
            payment.Status = res->getString("Status");
            payment.TransactionReference = res->getString("TransactionReference");
            payments.push_back(payment);
        }
        if (res) delete res;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting payments: " << e.what() << std::endl;
    }
    return payments;
}

bool PaymentManager::updatePaymentStatus(int paymentID, const std::string& status) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "UPDATE Payments SET Status = ? WHERE PaymentID = ?"
        );
        pstmt->setString(1, status);
        pstmt->setInt(2, paymentID);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    } catch (sql::SQLException& e) {
        std::cerr << "Error updating payment status: " << e.what() << std::endl;
        return false;
    }
}

bool PaymentManager::isRentalPaid(int rentalID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return false;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT SUM(Amount) as total FROM Payments WHERE RentalID = ? AND Status = 'Completed'"
        );
        pstmt->setInt(1, rentalID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        RentalManager rm;
        Rental* rental = rm.getRentalByID(rentalID);
        double totalDue = rental ? (rental->TotalAmount + rental->LateFee) : 0.0;
        delete rental;
        
        double totalPaid = 0.0;
        if (res && res->next()) {
            totalPaid = res->getDouble("total");
        }
        delete pstmt;
        if (res) delete res;
        return totalPaid >= totalDue;
    } catch (sql::SQLException& e) {
        std::cerr << "Error checking payment: " << e.what() << std::endl;
        return false;
    }
}

double PaymentManager::getTotalPaid(int rentalID) {
    try {
        sql::Connection* conn = DatabaseManager::getInstance().getConnection();
        if (!conn) {
            std::cerr << "Error: Database connection failed." << std::endl;
            return 0.0;
        }
        sql::PreparedStatement* pstmt = conn->prepareStatement(
            "SELECT SUM(Amount) as total FROM Payments WHERE RentalID = ? AND Status = 'Completed'"
        );
        pstmt->setInt(1, rentalID);
        sql::ResultSet* res = pstmt->executeQuery();
        
        double total = 0.0;
        if (res && res->next()) {
            total = res->getDouble("total");
        }
        delete pstmt;
        if (res) delete res;
        return total;
    } catch (sql::SQLException& e) {
        std::cerr << "Error getting total paid: " << e.what() << std::endl;
        return 0.0;
    }
}

void PaymentManager::generateReceipt(int paymentID) {
    Payment* payment = getPaymentByID(paymentID);
    if (!payment) {
        std::cout << "Payment not found." << std::endl;
        return;
    }
    
    RentalManager rm;
    Rental* rental = rm.getRentalByID(payment->RentalID);
    if (!rental) {
        delete payment;
        return;
    }
    
    CustomerManager cm;
    Customer* customer = cm.getCustomerByID(rental->CustomerID);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           DRESS RENTAL RECEIPT" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Payment ID: " << payment->PaymentID << std::endl;
    std::cout << "Rental ID: " << payment->RentalID << std::endl;
    if (customer) {
        std::cout << "Customer: " << customer->Name << std::endl;
        std::cout << "IC Number: " << customer->IC_Number << std::endl;
    }
    std::cout << "Payment Date: " << payment->PaymentDate << std::endl;
    std::cout << "Payment Method: " << payment->PaymentMethod << std::endl;
    std::cout << "Amount: RM " << std::fixed << std::setprecision(2) << payment->Amount << std::endl;
    std::cout << "Status: " << payment->Status << std::endl;
    if (!payment->TransactionReference.empty()) {
        std::cout << "Transaction Ref: " << payment->TransactionReference << std::endl;
    }
    std::cout << std::string(60, '=') << std::endl;
    
    delete payment;
    delete rental;
    if (customer) delete customer;
}

void PaymentManager::displayPayment(const Payment& payment) {
    std::cout << "\n+----------------------------------------+" << std::endl;
    std::cout << "| Payment ID: " << std::setw(25) << std::left << payment.PaymentID << "|" << std::endl;
    std::cout << "| Rental ID: " << std::setw(27) << std::left << payment.RentalID << "|" << std::endl;
    std::cout << "| Amount: RM " << std::setw(25) << std::left << std::fixed << std::setprecision(2) << payment.Amount << "|" << std::endl;
    std::cout << "| Payment Method: " << std::setw(22) << std::left << payment.PaymentMethod << "|" << std::endl;
    std::cout << "| Payment Date: " << std::setw(24) << std::left << payment.PaymentDate << "|" << std::endl;
    std::cout << "| Status: " << std::setw(29) << std::left << payment.Status << "|" << std::endl;
    if (!payment.TransactionReference.empty()) {
        std::cout << "| Transaction Ref: " << std::setw(21) << std::left << payment.TransactionReference << "|" << std::endl;
    }
    std::cout << "+----------------------------------------+" << std::endl;
}

void PaymentManager::displayAllPayments(const std::vector<Payment>& payments) {
    if (payments.empty()) {
        UIColors::printInfo("No payments found.");
        return;
    }
    
    std::cout << std::endl;
    UIColors::printCentered("PAYMENT LIST", 120, UIColors::BOLD + UIColors::CYAN);
    UIColors::printSeparator(120);
    std::cout << std::setw(10) << UIColors::colorize("Pay ID", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(12) << UIColors::colorize("Rental ID", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Amount", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(18) << UIColors::colorize("Method", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Date", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(15) << UIColors::colorize("Status", UIColors::BOLD + UIColors::YELLOW)
              << std::setw(20) << UIColors::colorize("Transaction Ref", UIColors::BOLD + UIColors::YELLOW) << std::endl;
    UIColors::printSeparator(120);
    
    for (const auto& payment : payments) {
        std::cout << std::setw(10) << payment.PaymentID
                  << std::setw(12) << payment.RentalID
                  << std::setw(15) << std::fixed << std::setprecision(2) << payment.Amount
                  << std::setw(18) << payment.PaymentMethod
                  << std::setw(15) << payment.PaymentDate
                  << std::setw(15) << payment.Status
                  << std::setw(20) << (payment.TransactionReference.length() > 18 ? 
                                       payment.TransactionReference.substr(0, 18) : 
                                       payment.TransactionReference) << std::endl;
    }
    UIColors::printSeparator(120);
}

