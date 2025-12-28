@echo off

if not exist build (
    echo "BUILD SCRIPT: Build folder missing! Generating Configuration!"

    cmake -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE --no-warn-unused-cli -S . -B ./build -G "Visual Studio 17 2022" -T host=x86 -A x64

    echo "BUILD SCRIPT: Build folder generated!"
)

:: Build command
cmake --build ./build --config Debug --target game -j 10 --

if %ERRORLEVEL% EQU 0 (
    echo "BUILD SCRIPT: BUILD successful! Running built program!"
    cd build
    game.exe
)else (
    echo "BUILD SCRIPT: Build failed!"
)

