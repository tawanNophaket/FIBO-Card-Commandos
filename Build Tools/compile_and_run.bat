@echo off
REM compile_and_run.bat - Windows script สำหรับ compile และรันเกม

echo 🔧 Compiling FIBO Card Commandos...

REM Compile the improved version with all new UI files
g++ -std=c++17 -Wall -Wextra -O2 main_improved.cpp Card.cpp Deck.cpp Player_Improved.cpp UIHelper.cpp MenuSystem.cpp -o fibo_card_commandos.exe

REM Check if compilation was successful
if %errorlevel% == 0 (
    echo ✅ Compilation successful!
    echo 🚀 Starting FIBO Card Commandos...
    echo.
    fibo_card_commandos.exe
) else (
    echo ❌ Compilation failed!
    pause
    exit /b 1
)

pause