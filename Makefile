# Compiler to use
CC = g++

# Compiler flags
CFLAGS = -Wall -g -std=c++11

# Target executable
TARGET = water_jug

# Directories
SRCDIR = src
INCDIR = include
BUILDDIR = build

# Source files
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/ida_star_search.cpp \
          $(SRCDIR)/a_star_search.cpp \
          $(SRCDIR)/ordenada_gulosa.cpp \
          $(SRCDIR)/backtrack.cpp \
          $(SRCDIR)/profundidade_largura.cpp \
          $(SRCDIR)/structure.cpp

# Object files
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))

# Header files
HEADERS = $(wildcard $(INCDIR)/*.hpp)

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@
	@echo "Compiled $< to $@"

# Clean up
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Phony targets
.PHONY: all clean