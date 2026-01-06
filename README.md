macOS Calculator App in Pure C using Cocoa

## Overview
A working calculator app built in pure C using Cocoa via `objc_msgSend` and Objective-C runtime, without using Objective-C language syntax.

## Features
- Calculator UI with 16 buttons (0-9, +, -, *, /, =)
- Display field showing results
- Working arithmetic operations (+, -, *, /)
- Cmd+Q keyboard shortcut to quit
- Window close button support

## Building

### Option 1: Using build script (recommended)
```bash
./build.sh
open Calculator.app
```

### Option 2: Manual compilation
```bash
gcc -o calculator calculator.c -framework Foundation -framework AppKit -lm
./calculator
```

Or as an app bundle:
```bash
mkdir -p Calculator.app/Contents/MacOS
gcc -o Calculator.app/Contents/MacOS/calculator calculator.c -framework Foundation -framework AppKit -lm
open Calculator.app
```

## Technical Details

### Menu Bar Rendering Solution
The macOS menu bar requires the app to be packaged as a `.app` bundle with:
- `Contents/MacOS/` - executable directory
- `Contents/Info.plist` - bundle metadata
- `Contents/PkgInfo` - package type identifier

Without this structure, Cocoa can create the menu programmatically (which is why Cmd+Q works), but macOS won't render it in the menu bar because:
1. The Windowserver/Aqua rendering system queries app bundles via Finder
2. It reads Info.plist for app metadata and bundle identifier
3. The Dock and menu bar use this information for display

The app bundle approach allows the menu bar to render visually while maintaining pure C implementation.

### Implementation Notes
- Uses objc_msgSend for all runtime calls
- Custom delegate classes for window and button events
- No Objective-C language features, pure C with runtime introspection
