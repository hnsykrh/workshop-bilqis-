#!/bin/bash
# Linux/Mac compilation script for Dress Rental Management System

echo "Compiling Dress Rental Management System..."

# Try to find MySQL Connector/C++ paths
MYSQL_INCLUDE="/usr/include/mysql-cppconn-8"
MYSQL_LIB="/usr/lib/x86_64-linux-gnu"

# Check if paths exist, adjust if needed
if [ ! -d "$MYSQL_INCLUDE" ]; then
    echo "Warning: MySQL include path not found at $MYSQL_INCLUDE"
    echo "Please adjust MYSQL_INCLUDE in this script"
fi

g++ -std=c++11 src/*.cpp -o rental_system -I"$MYSQL_INCLUDE" -I"include" -L"$MYSQL_LIB" -lmysqlcppconn

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Run ./rental_system to start the application."
else
    echo "Compilation failed!"
    echo "Please check:"
    echo "1. MySQL Connector/C++ is installed"
    echo "2. MySQL paths are correct"
    echo "3. g++ compiler is installed"
fi

