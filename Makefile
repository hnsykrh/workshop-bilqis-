# Makefile for Dress Rental Management System

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra

# MySQL Connector paths (adjust these based on your installation)
# Windows XAMPP example:
# MYSQL_INCLUDE = C:/xampp/mysql/include
# MYSQL_LIB = C:/xampp/mysql/lib

# Linux example:
# MYSQL_INCLUDE = /usr/include/mysql-cppconn-8
# MYSQL_LIB = /usr/lib/x86_64-linux-gnu

# Source files (from src directory)
SOURCES = src/main.cpp src/MenuHandlers.cpp src/DatabaseManager.cpp src/CustomerManager.cpp \
          src/DressManager.cpp src/RentalManager.cpp src/PaymentManager.cpp src/ReportManager.cpp \
          src/InputValidator.cpp src/UIColors.cpp src/AuthManager.cpp

# Object files
OBJECTS = $(SOURCES:src/%.cpp=%.o)

# Executable name
TARGET = rental_system

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -lmysqlcppconn

# Include directory for headers
INCLUDES = -Iinclude

# Compile source files to object files
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -f *.o *.exe

# Install dependencies (Linux)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libmysqlcppconn-dev g++

# Run the application
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean install-deps run

