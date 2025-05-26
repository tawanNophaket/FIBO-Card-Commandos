# Makefile for FIBO Card Commandos

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = fibo_card_commandos
SOURCES = main_improved.cpp Card.cpp Deck.cpp Player_Improved.cpp UIHelper.cpp MenuSystem.cpp

# Headers
HEADERS = Card.h Deck.h Player.h UIHelper.h MenuSystem.h json.hpp

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "🔧 Compiling FIBO Card Commandos..."
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)
	@echo "✅ Compilation successful!"

# Run the game
run: $(TARGET)
	@echo "🚀 Starting FIBO Card Commandos..."
	@echo ""
	./$(TARGET)

# Clean build files
clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(TARGET) $(TARGET).exe

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)
	@echo "🐛 Debug build completed!"

# Install dependencies (placeholder)
install-deps:
	@echo "📦 Installing dependencies..."
	@echo "ℹ️  This project only requires a C++17 compatible compiler"
	@echo "   Make sure you have g++ or clang++ installed"

# Help
help:
	@echo "📖 Available targets:"
	@echo "  all        - Build the game (default)"
	@echo "  run        - Build and run the game"
	@echo "  clean      - Remove build files"
	@echo "  debug      - Build with debug symbols"
	@echo "  install-deps - Show dependency information"
	@echo "  help       - Show this help message"

# Declare phony targets
.PHONY: all run clean debug install-deps help