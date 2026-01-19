# Makefile for Arduino Radar GUI

# Compiler
CC = gcc

# Target executable name
TARGET = radar

# Source files
SOURCES = gui.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Compiler flags
CFLAGS = -Wall -Wextra -O2

# SDL2 flags (using pkg-config for cross-platform compatibility)
SDL_CFLAGS = $(shell pkg-config --cflags sdl2 SDL2_ttf)
SDL_LIBS = $(shell pkg-config --libs sdl2 SDL2_ttf)

# Math library
LIBS = -lm

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(SDL_LIBS) $(LIBS)
	@echo "Build complete! Run with: ./$(TARGET) [serial_port]"
	@echo "Example: ./$(TARGET) /dev/tty.usbserial-0001"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

# Run the program (change port as needed)
run: $(TARGET)
	./$(TARGET) /dev/tty.usbserial-0001

# Clean build files
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Install dependencies (macOS with Homebrew)
install-deps-mac:
	brew install sdl2 sdl2_ttf pkg-config
	@echo "Dependencies installed!"

# Install dependencies (Ubuntu/Debian)
install-deps-linux:
	sudo apt-get update
	sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev pkg-config
	@echo "Dependencies installed!"

# Check if dependencies are installed
check-deps:
	@echo "Checking for SDL2..."
	@pkg-config --exists sdl2 && echo "✓ SDL2 found" || echo "✗ SDL2 not found - run 'make install-deps-mac'"
	@echo "Checking for SDL2_ttf..."
	@pkg-config --exists SDL2_ttf && echo "✓ SDL2_ttf found" || echo "✗ SDL2_ttf not found - run 'make install-deps-mac'"
	@echo "Checking for pkg-config..."
	@which pkg-config > /dev/null && echo "✓ pkg-config found" || echo "✗ pkg-config not found"

# Help target
help:
	@echo "Available targets:"
	@echo "  make              - Build the radar program"
	@echo "  make run          - Build and run the program"
	@echo "  make clean        - Remove build files"
	@echo "  make install-deps-mac   - Install dependencies on macOS"
	@echo "  make install-deps-linux - Install dependencies on Linux"
	@echo "  make check-deps   - Check if dependencies are installed"
	@echo "  make help         - Show this help message"

# Phony targets
.PHONY: all clean run install-deps-mac install-deps-linux check-deps help