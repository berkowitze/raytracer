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

run:
	sh -c 'rm -f raytracer && make && ./raytracer > out.ppm && open -g out.ppm && tput bel'

watch:
	@make run
	fswatch -o *.cpp *.h  | xargs -n1 sh -c 'rm -f raytracer && make && ./raytracer > out.ppm && open -g out.ppm && tput bel'