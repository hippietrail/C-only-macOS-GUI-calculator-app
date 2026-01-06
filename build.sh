#!/bin/bash
# Build script for macOS Calculator in Pure C

# Compile to raw executable (no bundle or plist needed)
gcc -o calculator calculator.c \
    -framework Foundation \
    -framework AppKit \
    -lm

# Ensure executable permissions
chmod +x calculator

echo "Build complete: calculator"
echo "Run with: ./calculator or open calculator"
