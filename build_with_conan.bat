@echo off
REM Build script for Colibri with Conan
REM This script demonstrates how to build the project using Conan for dependencies

REM Create build directory
if not exist build mkdir build

REM Install dependencies with Conan (output to build directory)
echo Installing dependencies with Conan...
conan install . --output-folder=build --build=missing

REM Configure with CMake using the Conan toolchain file
echo Configuring with CMake...
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

REM Build
echo Building...
cmake --build build --config Release

echo Build completed successfully!
pause 