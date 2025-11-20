-- Database Seeding Script
-- Adds sample data for testing (September 2025 - November 2025)
-- Run database_schema.sql first, then this file

USE dress_rental_db;

-- Insert Default Users (Password: admin123, staff123, customer123)
-- These hashes are computed using SHA-256 of "dress_rental_salt_2025_<password>"
-- The C++ AuthManager::hashPassword() uses the same algorithm
-- Run this query to get correct hashes: SELECT SHA2(CONCAT('dress_rental_salt_2025_', 'admin123'), 256);
INSERT INTO Users (Username, PasswordHash, Role, FullName, Email, Phone) VALUES
('admin', SHA2(CONCAT('dress_rental_salt_2025_', 'admin123'), 256), 'Administrator', 'Shop Administrator', 'admin@dressrental.com', '0123456789'),
('staff1', SHA2(CONCAT('dress_rental_salt_2025_', 'staff123'), 256), 'Staff', 'Staff Member One', 'staff1@dressrental.com', '0198765432'),
('customer1', SHA2(CONCAT('dress_rental_salt_2025_', 'customer123'), 256), 'Customer', 'Customer User One', 'customer1@email.com', '0134567890');

-- Clear existing data (optional - comment out if you want to keep existing data)
-- DELETE FROM Payments;
-- DELETE FROM RentalItems;
-- DELETE FROM Rentals;
-- DELETE FROM Dresses;
-- DELETE FROM Customers;

-- Insert Sample Customers (5 customers)
INSERT INTO Customers (Name, IC_Number, Phone, Email, Address, DateOfBirth) VALUES
('Ahmad bin Abdullah', '900101-01-1234', '0123456789', 'ahmad@email.com', '123 Jalan Merdeka, Kuala Lumpur', '1990-01-01'),
('Siti Nurhaliza', '850215-14-5678', '0198765432', 'siti@email.com', '456 Jalan Ampang, Selangor', '1985-02-15'),
('Lim Wei Ming', '920330-08-9012', '0112233445', 'lim@email.com', '789 Jalan Bukit Bintang, KL', '1992-03-30'),
('Priya Devi', '880425-10-3456', '0134567890', 'priya@email.com', '321 Jalan Tun Razak, KL', '1988-04-25'),
('Muhammad Firdaus', '910512-06-7890', '0145678901', 'firdaus@email.com', '654 Jalan Pudu, KL', '1991-05-12');

-- Insert Sample Dresses (15 dresses)
INSERT INTO Dresses (DressName, Category, Size, Color, RentalPrice, ConditionStatus, AvailabilityStatus) VALUES
('Elegant Evening Gown', 'Evening', 'M', 'Black', 150.00, 'Good', 'Available'),
('Princess Ball Gown', 'Formal', 'S', 'Pink', 200.00, 'Good', 'Available'),
('Classic Wedding Dress', 'Wedding', 'M', 'White', 300.00, 'Good', 'Available'),
('Vintage Tea Dress', 'Casual', 'L', 'Blue', 80.00, 'Good', 'Available'),
('Designer Cocktail Dress', 'Cocktail', 'S', 'Red', 120.00, 'Good', 'Available'),
('Traditional Baju Kurung', 'Traditional', 'M', 'Green', 100.00, 'Good', 'Available'),
('Modern Jumpsuit', 'Casual', 'L', 'Navy', 90.00, 'Good', 'Available'),
('Sparkly Party Dress', 'Party', 'S', 'Gold', 110.00, 'Good', 'Available'),
('Elegant Maxi Dress', 'Casual', 'M', 'Purple', 95.00, 'Good', 'Available'),
('Bridal Gown Premium', 'Wedding', 'L', 'Ivory', 350.00, 'Good', 'Available'),
('Floral Summer Dress', 'Casual', 'S', 'Yellow', 75.00, 'Good', 'Available'),
('Formal Business Dress', 'Formal', 'M', 'Gray', 130.00, 'Good', 'Available'),
('Vintage 50s Dress', 'Vintage', 'S', 'Teal', 140.00, 'Good', 'Available'),
('Designer Evening Dress', 'Evening', 'L', 'Silver', 180.00, 'Good', 'Available'),
('Traditional Cheongsam', 'Traditional', 'M', 'Red', 160.00, 'Good', 'Available');

-- Insert Rentals for September 2025 (5 rentals)
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-09-05', '2025-09-12', '2025-09-11', 1050.00, 0.00, 'Returned'),
(2, '2025-09-10', '2025-09-17', '2025-09-16', 800.00, 0.00, 'Returned'),
(3, '2025-09-15', '2025-09-22', '2025-09-21', 600.00, 0.00, 'Returned'),
(4, '2025-09-20', '2025-09-27', '2025-09-26', 450.00, 0.00, 'Returned'),
(5, '2025-09-25', '2025-10-02', '2025-10-01', 950.00, 0.00, 'Returned');

-- Insert Rental Items for September
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(1, 1, 150.00), (1, 3, 300.00), (1, 10, 350.00), (1, 14, 180.00), (1, 15, 160.00),
(2, 2, 200.00), (2, 6, 100.00), (2, 8, 110.00), (2, 12, 130.00), (2, 13, 140.00),
(3, 4, 80.00), (3, 7, 90.00), (3, 9, 95.00), (3, 11, 75.00),
(4, 5, 120.00), (4, 4, 80.00), (4, 9, 95.00), (4, 11, 75.00),
(5, 1, 150.00), (5, 3, 300.00), (5, 10, 350.00), (5, 14, 180.00);

-- Insert Payments for September
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(1, 1050.00, 'Cash', '2025-09-05', 'Completed', 'PAY-SEP-001'),
(2, 800.00, 'Credit Card', '2025-09-10', 'Completed', 'PAY-SEP-002'),
(3, 600.00, 'Debit Card', '2025-09-15', 'Completed', 'PAY-SEP-003'),
(4, 450.00, 'Online', '2025-09-20', 'Completed', 'PAY-SEP-004'),
(5, 950.00, 'Cash', '2025-09-25', 'Completed', 'PAY-SEP-005');

-- Insert Rentals for October 2025 (5 rentals)
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-10-03', '2025-10-10', '2025-10-09', 1200.00, 0.00, 'Returned'),
(2, '2025-10-08', '2025-10-15', '2025-10-14', 700.00, 0.00, 'Returned'),
(3, '2025-10-13', '2025-10-20', '2025-10-19', 550.00, 0.00, 'Returned'),
(4, '2025-10-18', '2025-10-25', '2025-10-24', 850.00, 0.00, 'Returned'),
(5, '2025-10-23', '2025-10-30', '2025-10-29', 1100.00, 0.00, 'Returned');

-- Insert Rental Items for October
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(6, 2, 200.00), (6, 3, 300.00), (6, 10, 350.00), (6, 14, 180.00), (6, 15, 160.00),
(7, 1, 150.00), (7, 5, 120.00), (7, 8, 110.00), (7, 12, 130.00), (7, 13, 140.00),
(8, 4, 80.00), (8, 7, 90.00), (8, 9, 95.00), (8, 11, 75.00),
(9, 1, 150.00), (9, 3, 300.00), (9, 10, 350.00), (9, 14, 180.00),
(10, 2, 200.00), (10, 6, 100.00), (10, 8, 110.00), (10, 12, 130.00), (10, 15, 160.00);

-- Insert Payments for October
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(6, 1200.00, 'Credit Card', '2025-10-03', 'Completed', 'PAY-OCT-001'),
(7, 700.00, 'Debit Card', '2025-10-08', 'Completed', 'PAY-OCT-002'),
(8, 550.00, 'Online', '2025-10-13', 'Completed', 'PAY-OCT-003'),
(9, 850.00, 'Cash', '2025-10-18', 'Completed', 'PAY-OCT-004'),
(10, 1100.00, 'Credit Card', '2025-10-23', 'Completed', 'PAY-OCT-005');

-- Insert Rentals for November 2025 (5 rentals)
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-11-01', '2025-11-08', '2025-11-07', 1000.00, 0.00, 'Returned'),
(2, '2025-11-06', '2025-11-13', '2025-11-12', 750.00, 0.00, 'Returned'),
(3, '2025-11-11', '2025-11-18', '2025-11-17', 650.00, 0.00, 'Returned'),
(4, '2025-11-16', '2025-11-23', '2025-11-22', 900.00, 0.00, 'Returned'),
(5, '2025-11-21', '2025-11-28', NULL, 1150.00, 0.00, 'Active');

-- Insert Rental Items for November
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(11, 1, 150.00), (11, 3, 300.00), (11, 10, 350.00), (11, 14, 180.00), (11, 15, 160.00),
(12, 2, 200.00), (12, 6, 100.00), (12, 8, 110.00), (12, 12, 130.00), (12, 13, 140.00),
(13, 4, 80.00), (13, 7, 90.00), (13, 9, 95.00), (13, 11, 75.00),
(14, 5, 120.00), (14, 4, 80.00), (14, 9, 95.00), (14, 11, 75.00), (14, 12, 130.00),
(15, 1, 150.00), (15, 3, 300.00), (15, 10, 350.00), (15, 14, 180.00), (15, 15, 160.00);

-- Insert Payments for November
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(11, 1000.00, 'Cash', '2025-11-01', 'Completed', 'PAY-NOV-001'),
(12, 750.00, 'Credit Card', '2025-11-06', 'Completed', 'PAY-NOV-002'),
(13, 650.00, 'Debit Card', '2025-11-11', 'Completed', 'PAY-NOV-003'),
(14, 900.00, 'Online', '2025-11-16', 'Completed', 'PAY-NOV-004'),
(15, 1150.00, 'Credit Card', '2025-11-21', 'Completed', 'PAY-NOV-005');

-- Update dress availability for active rental
UPDATE Dresses SET AvailabilityStatus = 'Rented' WHERE DressID IN (1, 3, 10, 14, 15);

