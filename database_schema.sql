-- Dress Rental Management System Database Schema
-- Database: dress_rental_db
-- User: root, Password: ""

CREATE DATABASE IF NOT EXISTS dress_rental_db;
USE dress_rental_db;

-- Users Table (for authentication)
CREATE TABLE IF NOT EXISTS Users (
    UserID INT AUTO_INCREMENT PRIMARY KEY,
    Username VARCHAR(50) UNIQUE NOT NULL,
    PasswordHash VARCHAR(64) NOT NULL,
    Role VARCHAR(20) NOT NULL DEFAULT 'Staff',
    FullName VARCHAR(100) NOT NULL,
    Email VARCHAR(100),
    Phone VARCHAR(20),
    IsActive BOOLEAN DEFAULT TRUE,
    LastLogin TIMESTAMP NULL,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_username (Username),
    INDEX idx_role (Role)
);

-- Customers Table
CREATE TABLE IF NOT EXISTS Customers (
    CustomerID INT AUTO_INCREMENT PRIMARY KEY,
    Name VARCHAR(100) NOT NULL,
    IC_Number VARCHAR(20) UNIQUE NOT NULL,
    Phone VARCHAR(20) NOT NULL,
    Email VARCHAR(100),
    Address TEXT,
    DateOfBirth DATE NOT NULL,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_ic (IC_Number),
    INDEX idx_phone (Phone)
);

-- Dresses Table
CREATE TABLE IF NOT EXISTS Dresses (
    DressID INT AUTO_INCREMENT PRIMARY KEY,
    DressName VARCHAR(100) NOT NULL,
    Category VARCHAR(50) NOT NULL,
    Size VARCHAR(10) NOT NULL,
    Color VARCHAR(30) NOT NULL,
    RentalPrice DECIMAL(10, 2) NOT NULL,
    ConditionStatus VARCHAR(20) DEFAULT 'Good',
    AvailabilityStatus VARCHAR(20) DEFAULT 'Available',
    CleaningStatus VARCHAR(20) DEFAULT 'Clean',
    StockQuantity INT DEFAULT 1,
    MinimumStockLevel INT DEFAULT 1,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_category (Category),
    INDEX idx_availability (AvailabilityStatus)
);

-- Rental Transactions Table
CREATE TABLE IF NOT EXISTS Rentals (
    RentalID INT AUTO_INCREMENT PRIMARY KEY,
    CustomerID INT NOT NULL,
    RentalDate DATE NOT NULL,
    DueDate DATE NOT NULL,
    ReturnDate DATE,
    TotalAmount DECIMAL(10, 2) NOT NULL,
    LateFee DECIMAL(10, 2) DEFAULT 0.00,
    Status VARCHAR(20) DEFAULT 'Active',
    CreatedBy INT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (CustomerID) REFERENCES Customers(CustomerID) ON DELETE CASCADE,
    FOREIGN KEY (CreatedBy) REFERENCES Users(UserID) ON DELETE SET NULL,
    INDEX idx_customer (CustomerID),
    INDEX idx_status (Status),
    INDEX idx_due_date (DueDate)
);

-- Rental Items Table (Many-to-Many: Rentals to Dresses)
CREATE TABLE IF NOT EXISTS RentalItems (
    RentalItemID INT AUTO_INCREMENT PRIMARY KEY,
    RentalID INT NOT NULL,
    DressID INT NOT NULL,
    RentalPrice DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (RentalID) REFERENCES Rentals(RentalID) ON DELETE CASCADE,
    FOREIGN KEY (DressID) REFERENCES Dresses(DressID) ON DELETE CASCADE,
    INDEX idx_rental (RentalID),
    INDEX idx_dress (DressID)
);

-- Payments Table
CREATE TABLE IF NOT EXISTS Payments (
    PaymentID INT AUTO_INCREMENT PRIMARY KEY,
    RentalID INT NOT NULL,
    Amount DECIMAL(10, 2) NOT NULL,
    PaymentMethod VARCHAR(20) NOT NULL,
    PaymentDate DATE NOT NULL,
    Status VARCHAR(20) DEFAULT 'Pending',
    TransactionReference VARCHAR(100),
    ProcessedBy INT,
    CreatedAt TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (RentalID) REFERENCES Rentals(RentalID) ON DELETE CASCADE,
    FOREIGN KEY (ProcessedBy) REFERENCES Users(UserID) ON DELETE SET NULL,
    INDEX idx_rental (RentalID),
    INDEX idx_status (Status),
    INDEX idx_payment_date (PaymentDate)
);

-- Activity Log Table (for audit trail)
CREATE TABLE IF NOT EXISTS ActivityLog (
    LogID INT AUTO_INCREMENT PRIMARY KEY,
    UserID INT,
    Action VARCHAR(100) NOT NULL,
    TableName VARCHAR(50),
    RecordID INT,
    Details TEXT,
    Timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (UserID) REFERENCES Users(UserID) ON DELETE SET NULL,
    INDEX idx_user (UserID),
    INDEX idx_timestamp (Timestamp)
);
