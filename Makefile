# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra
EIGEN_PATH := /usr/local/include/eigen-5.0.0

TARGET := main
SOURCES := main.cpp
HEADERS := hopfield.hpp pattern.hpp PBM.hpp

# Default target
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -I$(EIGEN_PATH) $(SOURCES) -o $(TARGET)

# Run target
run: $(TARGET)
	./$(TARGET)

# Clean target
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: run clean