# Directories
SRC_DIR = ../../src
BUILD_DIR = ../../build

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++20 -O3 -march=native -I$(SRC_DIR)  # Add src directory for headers

# Linker flags (Optional for additional linker settings, currently empty)
LDFLAGS =  # No special linker flags for this project

# Libraries to link with
LIBS = -lzstd  # Link with Zstandard library

# Source files and target executable
SRC_FILES = \
	$(SRC_DIR)/general.cpp $(SRC_DIR)/cli.cpp \
	$(SRC_DIR)/graph.cpp \
	$(SRC_DIR)/rootedtree.cpp \
	$(SRC_DIR)/treedecomp.cpp
TARGET = $(BUILD_DIR)/mwis_smart

# Default target: Build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): main.cpp $(SRC_FILES)
	$(CXX) $(CXXFLAGS) main.cpp $(SRC_FILES) \
	-o $(TARGET) \
	$(LDFLAGS) $(LIBS)

# Clean the build
clean:
	rm -f $(TARGET)