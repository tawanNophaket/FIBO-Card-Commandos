# Makefile for FIBO Card Commandos (แก้ไขแล้ว)

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = fibo_card_commandos

# Source files with correct paths
GAME_CORE_DIR = Game\ Core
UI_SYSTEM_DIR = UI\ System
DATA_DIR = Data

SOURCES = $(GAME_CORE_DIR)/main_improved.cpp \
          $(GAME_CORE_DIR)/Card.cpp \
          $(GAME_CORE_DIR)/Deck.cpp \
          $(GAME_CORE_DIR)/Player_Improved.cpp \
          $(UI_SYSTEM_DIR)/UIHelper.cpp \
          $(UI_SYSTEM_DIR)/MenuSystem.cpp

# Headers
HEADERS = $(GAME_CORE_DIR)/Card.h \
          $(GAME_CORE_DIR)/Deck.h \
          $(GAME_CORE_DIR)/Player.h \
          $(UI_SYSTEM_DIR)/UIHelper.h \
          $(UI_SYSTEM_DIR)/MenuSystem.h \
          json.hpp

# Include directories
INCLUDES = -I"$(GAME_CORE_DIR)" -I"$(UI_SYSTEM_DIR)" -I.

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "🔧 Compiling FIBO Card Commandos..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)
	@echo "✅ Compilation successful!"
	@cp "$(DATA_DIR)/cards.json" "cards.json" 2>/dev/null || true

# Run the game
run: $(TARGET)
	@echo "🚀 Starting FIBO Card Commandos..."
	@echo ""
	./$(TARGET)

# Clean build files
clean:
	@echo "🧹 Cleaning build files..."
	rm -f $(TARGET) $(TARGET).exe cards.json

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)
	@echo "🐛 Debug build completed!"

# Windows-specific build
windows: CXXFLAGS += -static
windows: $(TARGET).exe

$(TARGET).exe: $(SOURCES) $(HEADERS)
	@echo "🔧 Compiling for Windows..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET).exe
	@echo "✅ Windows compilation successful!"
	@cp "$(DATA_DIR)/cards.json" "cards.json" 2>nul || echo.

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
	@echo "  windows    - Build Windows executable"
	@echo "  install-deps - Show dependency information"
	@echo "  help       - Show this help message"

# Declare phony targets
.PHONY: all run clean debug windows install-deps help