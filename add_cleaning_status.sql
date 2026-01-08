-- Migration script to add CleaningStatus column to existing Dresses table
-- Run this script if you have an existing database

USE dress_rental_db;

-- Add CleaningStatus column if it doesn't exist
ALTER TABLE Dresses 
ADD COLUMN IF NOT EXISTS CleaningStatus VARCHAR(20) DEFAULT 'Clean' 
AFTER AvailabilityStatus;

-- Update existing records to have 'Clean' as default
UPDATE Dresses 
SET CleaningStatus = 'Clean' 
WHERE CleaningStatus IS NULL OR CleaningStatus = '';

