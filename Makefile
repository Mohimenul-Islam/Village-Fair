# Makefile for OpenGL Project
# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -std=c++11

# Libraries
LIBS = -lGL -lGLU -lglut -lm

# Target executable
TARGET = ferris_wheel

# Source files
SOURCES = main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all run clean
