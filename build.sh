#!/bin/bash
# Build script for macOS Calculator in Pure C

# Create app bundle structure
mkdir -p Calculator.app/Contents/MacOS

# Compile the calculator
gcc -o Calculator.app/Contents/MacOS/calculator calculator.c \
    -framework Foundation \
    -framework AppKit \
    -lm

# Ensure executable permissions
chmod +x Calculator.app/Contents/MacOS/calculator

echo "Build complete: Calculator.app"
echo "Run with: open Calculator.app"
echo "Or run directly: ./Calculator.app/Contents/MacOS/calculator"
