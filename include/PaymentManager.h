#ifndef PAYMENTMANAGER_H
#define PAYMENTMANAGER_H

#include "DatabaseManager.h"
#include "RentalManager.h"
#include <string>
#include <vector>

struct Payment {
    int PaymentID;
    int RentalID;
    double Amount;
    std::string PaymentMethod;
    std::string PaymentDate;
    std::string Status;
    std::string TransactionReference;
};

class PaymentManager {
public:
    bool createPayment(int rentalID, double amount, const std::string& paymentMethod, 
                      const std::string& paymentDate, const std::string& transactionRef = "");
    Payment* getPaymentByID(int paymentID);
    std::vector<Payment> getPaymentsByRental(int rentalID);
    std::vector<Payment> getAllPayments();
    bool updatePaymentStatus(int paymentID, const std::string& status);
    bool isRentalPaid(int rentalID);
    double getTotalPaid(int rentalID);
    void generateReceipt(int paymentID);
    void displayPayment(const Payment& payment);
    void displayAllPayments(const std::vector<Payment>& payments);
    bool validatePaymentMethod(const std::string& method);
};

#endif

