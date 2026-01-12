-- Check and Fix Password Script
-- This script checks current passwords and updates them if needed
-- Run this in MySQL to update existing user passwords

USE dress_rental_db;

-- Check current password hashes
SELECT 
    Username, 
    Role,
    LEFT(PasswordHash, 20) as PasswordHash_Preview,
    CASE 
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256) THEN 'CORRECT (Admin2025!@)'
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256) THEN 'CORRECT (Staff2025!@)'
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'admin123'), 256) THEN 'OLD (admin123) - NEEDS UPDATE'
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'staff123'), 256) THEN 'OLD (staff123) - NEEDS UPDATE'
        ELSE 'UNKNOWN'
    END as PasswordStatus
FROM Users 
WHERE Username IN ('admin', 'staff1');

-- Update Admin password (Admin2025!@)
UPDATE Users 
SET PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256)
WHERE Username = 'admin';

-- Update Staff password (Staff2025!@)
UPDATE Users 
SET PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256)
WHERE Username = 'staff1';

-- Verify the updates
SELECT 
    Username, 
    Role,
    LEFT(PasswordHash, 20) as PasswordHash_Preview,
    CASE 
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256) THEN 'Admin2025!@'
        WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256) THEN 'Staff2025!@'
        ELSE 'UNKNOWN'
    END as CurrentPassword
FROM Users 
WHERE Username IN ('admin', 'staff1');


