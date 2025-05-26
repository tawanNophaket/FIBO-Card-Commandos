#!/bin/bash
# compile_and_run.sh - Script สำหรับ compile และรันเกม

echo "🔧 Compiling FIBO Card Commandos..."

# Compile the improved version with all new UI files
g++ -std=c++17 -Wall -Wextra -O2 \
    Main.cpp \
    Card.cpp \
    Deck.cpp \
    Player.cpp \
    UIHelper.cpp \
    MenuSystem.cpp \
    -o fibo_card_commandos

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "🚀 Starting FIBO Card Commandos..."
    echo ""
    ./fibo_card_commandos
else
    echo "❌ Compilation failed!"
    exit 1
fi