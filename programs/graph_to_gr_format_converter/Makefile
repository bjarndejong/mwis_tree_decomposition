# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++20 -O3 -march=native

# Directories
BUILD_DIR = ../../build

# Source files and target executable
SRC_FILES =
TARGET = $(BUILD_DIR)/graph_to_gr_format_converter

# Default target: Build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

# Clean the build
clean:
	rm -f $(TARGET)