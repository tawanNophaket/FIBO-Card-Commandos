# Makefile สำหรับ FIBO Card Commandos
# ไฟล์นี้ใช้สำหรับการคอมไพล์และรันเกม

# การตั้งค่าคอมไพเลอร์
CXX = g++                                    # เลือกใช้ g++ เป็นคอมไพเลอร์
CXXFLAGS = -std=c++17 -Wall -Wextra -O2     # ตั้งค่า flags สำหรับการคอมไพล์
TARGET = fibo_card_commandos                 # ชื่อไฟล์เป้าหมายที่จะสร้าง

# ไดเรกทอรีของซอร์สโค้ด
GAME_CORE_DIR = Game\ Core                   # โฟลเดอร์เก็บโค้ดหลักของเกม
UI_SYSTEM_DIR = UI\ System                   # โฟลเดอร์เก็บโค้ดส่วน UI
DATA_DIR = Data                              # โฟลเดอร์เก็บข้อมูล

# รายชื่อไฟล์ซอร์สโค้ด
SOURCES = $(GAME_CORE_DIR)/Main.cpp \
          $(GAME_CORE_DIR)/Card.cpp \
          $(GAME_CORE_DIR)/Deck.cpp \
          $(GAME_CORE_DIR)/Player.cpp \
          $(UI_SYSTEM_DIR)/UIHelper.cpp \
          $(UI_SYSTEM_DIR)/MenuSystem.cpp

# รายชื่อไฟล์ส่วนหัว
HEADERS = $(GAME_CORE_DIR)/Card.h \
          $(GAME_CORE_DIR)/Deck.h \
          $(GAME_CORE_DIR)/Player.h \
          $(UI_SYSTEM_DIR)/UIHelper.h \
          $(UI_SYSTEM_DIR)/MenuSystem.h \
          json.hpp

# ไดเรกทอรีที่ต้องรวมเข้าไป
INCLUDES = -I"$(GAME_CORE_DIR)" -I"$(UI_SYSTEM_DIR)" -I.

# เป้าหมายหลัก
all: $(TARGET)

# การสร้างไฟล์เป้าหมาย
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "🔧 กำลังคอมไพล์ FIBO Card Commandos..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)
	@echo "✅ คอมไพล์สำเร็จ!"
	@cp "$(DATA_DIR)/cards.json" "cards.json" 2>/dev/null || true

# รันเกม
run: $(TARGET)
	@echo "🚀 กำลังเริ่มเกม FIBO Card Commandos..."
	@echo ""
	./$(TARGET)

# ล้างไฟล์ที่สร้างจากการ build
clean:
	@echo "🧹 กำลังล้างไฟล์ build..."
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