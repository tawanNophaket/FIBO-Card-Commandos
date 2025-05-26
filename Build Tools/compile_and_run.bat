@echo off
REM compile_and_run.bat - Windows script สำหรับ compile และรันเกม (แก้ไขแล้ว)

echo 🔧 Compiling FIBO Card Commandos...

REM Navigate to project root and compile with correct paths
cd /d "%~dp0\.."

REM Compile with correct file paths
g++ -std=c++17 -Wall -Wextra -O2 ^
    "Game Core\Main.cpp" ^
    "Game Core\Card.cpp" ^
    "Game Core\Deck.cpp" ^
    "Game Core\Player.cpp" ^
    "UI System\UIHelper.cpp" ^
    "UI System\MenuSystem.cpp" ^
    -I"Game Core" ^
    -I"UI System" ^
    -I. ^
    -o fibo_card_commandos.exe

REM Check if compilation was successful
if %errorlevel% == 0 (
    echo ✅ Compilation successful!
    echo 🚀 Starting FIBO Card Commandos...
    echo.

    REM Copy cards.json to working directory
    copy "Data\cards.json" "cards.json" >nul 2>&1

    fibo_card_commandos.exe
) else (
    echo ❌ Compilation failed!
    echo Check that all source files exist and g++ is installed
    pause
    exit /b 1
)

pause