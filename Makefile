# The compiler to use
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -Wall

FLAGS = --bvh

# Source files
SRC = Raytracer.cpp

# Output executable
OUT = raytracer

OUTPUT_FILE = out/$(shell date +%s).ppm

# Rule for compiling the program
$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

# Clean rule to remove the executable
clean:
	@echo "Cleaning up..."
	@rm -f $(OUT)

run_one_process:
	@echo "./raytracer $(FLAGS) > $(OUTPUT_FILE)"
	@sh -c 'make clean && make && time ./raytracer $(FLAGS) > $(OUTPUT_FILE) && open -g $(OUTPUT_FILE) && tput bel'

run:
	@echo "python multiprocess.py"
	@sh -c 'make clean && make && python multiprocess.py && tput bel'

watch:
	@make run
	@fswatch -o *.cpp *.h  | xargs -n1 make run
