-- Update Default User Passwords
-- New passwords that fulfill all requirements:
-- Admin: Admin2025!@
-- Staff: Staff2025!@
-- Requirements: 8+ chars, uppercase, lowercase, digit, special character

USE dress_rental_db;

-- Update Admin password (Admin2025!@)
-- Hash: SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256)
UPDATE Users 
SET PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256)
WHERE Username = 'admin';

-- Update Staff password (Staff2025!@)
-- Hash: SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256)
UPDATE Users 
SET PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256)
WHERE Username = 'staff1';

-- Verify the updates
SELECT Username, Role, FullName, 
       CASE 
           WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Admin2025!@'), 256) THEN 'Admin2025!@'
           WHEN PasswordHash = SHA2(CONCAT('dress_rental_salt_2025_', 'Staff2025!@'), 256) THEN 'Staff2025!@'
           ELSE 'Other'
       END as PasswordSet
FROM Users 
WHERE Username IN ('admin', 'staff1');

