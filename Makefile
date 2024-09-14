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
	rm -f $(OUT)

run:
	sh -c 'rm -f raytracer && make && ./raytracer $(FLAGS) > $(OUTPUT_FILE) && open -g $(OUTPUT_FILE) && tput bel'

watch:
	@make run
	fswatch -o *.cpp *.h  | xargs -n1 sh -c 'rm -f raytracer && make && ./raytracer $(FLAGS) > $(OUTPUT_FILE) && open -g $(OUTPUT_FILE) && tput bel'

output:
	rm -f raytracer
	make
	time ./raytracer $(FLAGS) > $(OUTPUT_FILE)

	BASENAME=$(basename $(OUTPUT_FILE) .ppm)
	convert $(OUTPUT_FILE) $$BASENAME.png

