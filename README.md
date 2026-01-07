macOS Calculator App in Pure C with No Bundle or Plist Required

## Overview

A working calculator application built in **pure C** using Cocoa via `objc_msgSend` and the Objective-C runtime—without using Objective-C language syntax and without requiring macOS bundle or plist files.

This project demonstrates:
- **GUI development outside Xcode** in pure C (applicable to any systems language)
- **Native macOS menu bar support** without bundle/plist (contrary to common misconceptions online)
- **Direct Cocoa integration** using only the Objective-C runtime introspection API

## Features

- Calculator UI with 16 buttons (0-9, +, -, *, /, =)
- Display field showing arithmetic results  
- Working operations: +, -, *, /
- Native menu bar with Cmd+Q to quit (no bundle required)
- Window close button to exit

## Building

Both single executable and app bundle approaches work:

### Single Executable (Recommended)

```bash
./build.sh
./calculator
```

Or launch via Finder/Dock:
```bash
open calculator
```

### Manual Compilation

```bash
gcc -o calculator calculator.c -framework Foundation -framework AppKit -lm
./calculator
```

### As an App Bundle (Optional)

If you prefer the app bundle experience:

```bash
mkdir -p Calculator.app/Contents/MacOS
gcc -o Calculator.app/Contents/MacOS/calculator calculator.c -framework Foundation -framework AppKit -lm
open Calculator.app
```

## Technical Details

### Implementation
- Uses `objc_msgSend()` for all runtime method calls
- Dynamically creates delegate classes for window and button events
- No Objective-C language features—pure C with runtime introspection
- Handles ARM64 and x86_64 calling conventions for `objc_msgSend`

### Menu Bar Without Bundle

The key insight is that macOS doesn't require a `.app` bundle to display the menu bar. The app simply needs to:
1. Set an app delegate before calling `finishLaunching()`
2. Create UI during the `applicationDidFinishLaunching:` callback (not after)
3. Call `activateIgnoringOtherApps:` for immediate foreground rendering
4. Enter the event loop with `run()`

This contradicts the common misconception that bundles are essential for menu visibility.

## Project Context

This started as a learning exercise in:
- Using **Amp Free** with the **Beads** issue tracker for the first time
- Building a macOS GUI entirely in C outside the Xcode ecosystem
- Proving that bundles/plists are not technically required for visible menus (despite online claims to the contrary)

The result is a minimal proof-of-concept showing that modern macOS app development can happen outside Xcode, in pure C, with full native UI integration.

---

Made with [Amp Free](https://ampcode.com/free) and [Beads](https://github.com/steveyegge/beads)
