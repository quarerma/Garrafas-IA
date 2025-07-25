# Compiler to use
CC = g++

# Compiler flags
CFLAGS = -Wall -g -std=c++11

# Target executables
TARGETS = water_jug backtrack

# Object files
OBJECTS = ida_star_search.o backtrack.o

# Default target
all: $(TARGETS)

# Link object files to create executables
water_jug: ida_star_search.o
	$(CC) $(CFLAGS) -o water_jug ida_star_search.o

backtrack: backtrack.o
	$(CC) $(CFLAGS) -o backtrack backtrack.o

# Compile source files to object files
ida_star_search.o: ida_star_search.cpp structure.hpp
	$(CC) $(CFLAGS) -c ida_star_search.cpp

backtrack.o: backtrack.cpp backtrack.hpp structure.hpp
	$(CC) $(CFLAGS) -c backtrack.cpp

# Clean up
clean:
	rm -f $(TARGETS) $(OBJECTS)

# Phony targets
.PHONY: all clean