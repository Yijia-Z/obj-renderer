# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17
LDFLAGS = -lglfw -lGLEW -lGL

# Source files
SOURCES = main.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable
EXECUTABLE = myapp

# Default target
all: $(EXECUTABLE)

# Build executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Run the executable
run: $(EXECUTABLE)
	./$(EXECUTABLE)