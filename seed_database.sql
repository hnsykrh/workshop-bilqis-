-- Database Seeding Script
-- Adds sample data for testing (September 2025 - November 2025)
-- Run database_schema.sql first, then this file

USE dress_rental_db;

-- Insert Default Users (Password: admin123, staff123)
-- These hashes are computed using SHA-256 of "dress_rental_salt_2025_<password>"
-- The C++ AuthManager::hashPassword() uses the same algorithm
-- Run this query to get correct hashes: SELECT SHA2(CONCAT('dress_rental_salt_2025_', 'admin123'), 256);
INSERT INTO Users (Username, PasswordHash, Role, FullName, Email, Phone) VALUES
('admin', SHA2(CONCAT('dress_rental_salt_2025_', 'admin123'), 256), 'Administrator', 'Shop Administrator', 'admin@dressrental.com', '0123456789'),
('staff1', SHA2(CONCAT('dress_rental_salt_2025_', 'staff123'), 256), 'Staff', 'Staff Member One', 'staff1@dressrental.com', '0198765432');

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

-- Insert Sample Dresses (15 dresses) - 50% Available, 50% Rented
INSERT INTO Dresses (DressName, Category, Size, Color, RentalPrice, ConditionStatus, AvailabilityStatus) VALUES
('Elegant Evening Gown', 'Evening', 'M', 'Black', 150.00, 'Good', 'Rented'),
('Princess Ball Gown', 'Formal', 'S', 'Pink', 200.00, 'Good', 'Available'),
('Classic Wedding Dress', 'Wedding', 'M', 'White', 300.00, 'Good', 'Rented'),
('Vintage Tea Dress', 'Casual', 'L', 'Blue', 80.00, 'Good', 'Available'),
('Designer Cocktail Dress', 'Cocktail', 'S', 'Red', 120.00, 'Good', 'Rented'),
('Traditional Baju Kurung', 'Traditional', 'M', 'Green', 100.00, 'Good', 'Available'),
('Modern Jumpsuit', 'Casual', 'L', 'Navy', 90.00, 'Good', 'Rented'),
('Sparkly Party Dress', 'Party', 'S', 'Gold', 110.00, 'Good', 'Available'),
('Elegant Maxi Dress', 'Casual', 'M', 'Purple', 95.00, 'Good', 'Rented'),
('Bridal Gown Premium', 'Wedding', 'L', 'Ivory', 350.00, 'Good', 'Available'),
('Floral Summer Dress', 'Casual', 'S', 'Yellow', 75.00, 'Good', 'Rented'),
('Formal Business Dress', 'Formal', 'M', 'Gray', 130.00, 'Good', 'Available'),
('Vintage 50s Dress', 'Vintage', 'S', 'Teal', 140.00, 'Good', 'Rented'),
('Designer Evening Dress', 'Evening', 'L', 'Silver', 180.00, 'Good', 'Available'),
('Traditional Cheongsam', 'Traditional', 'M', 'Red', 160.00, 'Good', 'Rented');

-- Insert Rentals for September 2025 (5 rentals)
-- Rental 1: 7 days (Sep 5-12), Items: 150+300+350+180+160 = 1140/day, Total: 7980
-- Rental 2: 7 days (Sep 10-17), Items: 200+100+110+130+140 = 680/day, Total: 4760
-- Rental 3: 7 days (Sep 15-22), Items: 80+90+95+75 = 340/day, Total: 2380
-- Rental 4: 7 days (Sep 20-27), Items: 120+80+95+75 = 370/day, Total: 2590
-- Rental 5: 7 days (Sep 25-Oct 2), Items: 150+300+350+180 = 980/day, Total: 6860
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-09-05', '2025-09-12', '2025-09-11', 7980.00, 0.00, 'Returned'),
(2, '2025-09-10', '2025-09-17', '2025-09-16', 4760.00, 0.00, 'Returned'),
(3, '2025-09-15', '2025-09-22', '2025-09-21', 2380.00, 0.00, 'Returned'),
(4, '2025-09-20', '2025-09-27', '2025-09-26', 2590.00, 0.00, 'Returned'),
(5, '2025-09-25', '2025-10-02', '2025-10-01', 6860.00, 0.00, 'Returned');

-- Insert Rental Items for September (RentalPrice is daily rate)
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(1, 1, 150.00), (1, 3, 300.00), (1, 10, 350.00), (1, 14, 180.00), (1, 15, 160.00),
(2, 2, 200.00), (2, 6, 100.00), (2, 8, 110.00), (2, 12, 130.00), (2, 13, 140.00),
(3, 4, 80.00), (3, 7, 90.00), (3, 9, 95.00), (3, 11, 75.00),
(4, 5, 120.00), (4, 4, 80.00), (4, 9, 95.00), (4, 11, 75.00),
(5, 1, 150.00), (5, 3, 300.00), (5, 10, 350.00), (5, 14, 180.00);

-- Insert Payments for September
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(1, 7980.00, 'Cash', '2025-09-05', 'Completed', 'PAY-SEP-001'),
(2, 4760.00, 'Credit Card', '2025-09-10', 'Completed', 'PAY-SEP-002'),
(3, 2380.00, 'Debit Card', '2025-09-15', 'Completed', 'PAY-SEP-003'),
(4, 2590.00, 'Online', '2025-09-20', 'Completed', 'PAY-SEP-004'),
(5, 6860.00, 'Cash', '2025-09-25', 'Completed', 'PAY-SEP-005');

-- Insert Rentals for October 2025 (5 rentals)
-- Rental 6: 7 days (Oct 3-10), Items: 200+300+350+180+160 = 1190/day, Total: 8330
-- Rental 7: 7 days (Oct 8-15), Items: 150+120+110+130+140 = 650/day, Total: 4550
-- Rental 8: 7 days (Oct 13-20), Items: 80+90+95+75 = 340/day, Total: 2380
-- Rental 9: 7 days (Oct 18-25), Items: 150+300+350+180 = 980/day, Total: 6860
-- Rental 10: 7 days (Oct 23-30), Items: 200+100+110+130+160 = 700/day, Total: 4900
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-10-03', '2025-10-10', '2025-10-09', 8330.00, 0.00, 'Returned'),
(2, '2025-10-08', '2025-10-15', '2025-10-14', 4550.00, 0.00, 'Returned'),
(3, '2025-10-13', '2025-10-20', '2025-10-19', 2380.00, 0.00, 'Returned'),
(4, '2025-10-18', '2025-10-25', '2025-10-24', 6860.00, 0.00, 'Returned'),
(5, '2025-10-23', '2025-10-30', '2025-10-29', 4900.00, 0.00, 'Returned');

-- Insert Rental Items for October (RentalPrice is daily rate)
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(6, 2, 200.00), (6, 3, 300.00), (6, 10, 350.00), (6, 14, 180.00), (6, 15, 160.00),
(7, 1, 150.00), (7, 5, 120.00), (7, 8, 110.00), (7, 12, 130.00), (7, 13, 140.00),
(8, 4, 80.00), (8, 7, 90.00), (8, 9, 95.00), (8, 11, 75.00),
(9, 1, 150.00), (9, 3, 300.00), (9, 10, 350.00), (9, 14, 180.00),
(10, 2, 200.00), (10, 6, 100.00), (10, 8, 110.00), (10, 12, 130.00), (10, 15, 160.00);

-- Insert Payments for October
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(6, 8330.00, 'Credit Card', '2025-10-03', 'Completed', 'PAY-OCT-001'),
(7, 4550.00, 'Debit Card', '2025-10-08', 'Completed', 'PAY-OCT-002'),
(8, 2380.00, 'Online', '2025-10-13', 'Completed', 'PAY-OCT-003'),
(9, 6860.00, 'Cash', '2025-10-18', 'Completed', 'PAY-OCT-004'),
(10, 4900.00, 'Credit Card', '2025-10-23', 'Completed', 'PAY-OCT-005');

-- Insert Rentals for November 2025 (5 rentals)
-- Rental 11: 7 days (Nov 1-8), Items: 150+300+350+180+160 = 1140/day, Total: 7980
-- Rental 12: 7 days (Nov 6-13), Items: 200+100+110+130+140 = 680/day, Total: 4760
-- Rental 13: 7 days (Nov 11-18), Items: 80+90+95+75 = 340/day, Total: 2380
-- Rental 14: 7 days (Nov 16-23), Items: 120+80+95+75+130 = 500/day, Total: 3500
-- Rental 15: 7 days (Nov 21-28), Items: 150+300+350+180+160 = 1140/day, Total: 7980 (Active)
INSERT INTO Rentals (CustomerID, RentalDate, DueDate, ReturnDate, TotalAmount, LateFee, Status) VALUES
(1, '2025-11-01', '2025-11-08', '2025-11-07', 7980.00, 0.00, 'Returned'),
(2, '2025-11-06', '2025-11-13', '2025-11-12', 4760.00, 0.00, 'Returned'),
(3, '2025-11-11', '2025-11-18', '2025-11-17', 2380.00, 0.00, 'Returned'),
(4, '2025-11-16', '2025-11-23', '2025-11-22', 3500.00, 0.00, 'Returned'),
(5, '2025-11-21', '2025-11-28', NULL, 7980.00, 0.00, 'Active');

-- Insert Rental Items for November (RentalPrice is daily rate)
INSERT INTO RentalItems (RentalID, DressID, RentalPrice) VALUES
(11, 1, 150.00), (11, 3, 300.00), (11, 10, 350.00), (11, 14, 180.00), (11, 15, 160.00),
(12, 2, 200.00), (12, 6, 100.00), (12, 8, 110.00), (12, 12, 130.00), (12, 13, 140.00),
(13, 4, 80.00), (13, 7, 90.00), (13, 9, 95.00), (13, 11, 75.00),
(14, 5, 120.00), (14, 4, 80.00), (14, 9, 95.00), (14, 11, 75.00), (14, 12, 130.00),
(15, 1, 150.00), (15, 3, 300.00), (15, 10, 350.00), (15, 14, 180.00), (15, 15, 160.00);

-- Insert Payments for November
INSERT INTO Payments (RentalID, Amount, PaymentMethod, PaymentDate, Status, TransactionReference) VALUES
(11, 7980.00, 'Cash', '2025-11-01', 'Completed', 'PAY-NOV-001'),
(12, 4760.00, 'Credit Card', '2025-11-06', 'Completed', 'PAY-NOV-002'),
(13, 2380.00, 'Debit Card', '2025-11-11', 'Completed', 'PAY-NOV-003'),
(14, 3500.00, 'Online', '2025-11-16', 'Completed', 'PAY-NOV-004'),
(15, 7980.00, 'Credit Card', '2025-11-21', 'Completed', 'PAY-NOV-005');

-- Update dress availability for active rental
UPDATE Dresses SET AvailabilityStatus = 'Rented' WHERE DressID IN (1, 3, 10, 14, 15);

