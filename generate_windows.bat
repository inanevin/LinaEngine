@echo off
setlocal

:: Set up paths
set BUILD_DIR=build_x64
set GENERATOR="Visual Studio 19 2022"
set TOOLCHAIN_FILE=CMake/Toolchain.cmake

:: Create the build directory if it doesn't exist
if not exist %BUILD_DIR% mkdir %BUILD_DIR%

:: Run CMake
cmake -B %BUILD_DIR% -G %GENERATOR% -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE%

:: Build the project
:: cmake --build %BUILD_DIR% --config Release

echo Build complete.
pause