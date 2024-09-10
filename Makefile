# The compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall

# Source files
SRC = Raytracer.cpp

# Output executable
OUT = raytracer

# Rule for compiling the program
$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

# Clean rule to remove the executable
clean:
	rm -f $(OUT)

