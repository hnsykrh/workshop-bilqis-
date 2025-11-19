@echo off
REM Windows compilation script for Dress Rental Management System
REM Adjust paths based on your XAMPP installation

set MYSQL_INCLUDE=C:\xampp\mysql\include
set MYSQL_LIB=C:\xampp\mysql\lib

echo Compiling Dress Rental Management System...
g++ -std=c++11 src\*.cpp -o rental_system.exe -I"%MYSQL_INCLUDE%" -I"include" -L"%MYSQL_LIB%" -lmysqlcppconn

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run rental_system.exe to start the application.
) else (
    echo Compilation failed!
    echo Please check:
    echo 1. MySQL Connector/C++ is installed
    echo 2. XAMPP MySQL paths are correct
    echo 3. g++ compiler is in PATH
)

pause

