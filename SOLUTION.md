# macOS Calculator in Pure C Without Bundle or Plist

## Goal
Demonstrate a minimal macOS GUI application written in pure C that:
- Creates windows and UI elements programmatically
- Sets up menus without requiring an app bundle or Info.plist
- Uses only Cocoa frameworks via `objc_msgSend` calls
- Compiles to a raw executable

## Solution

### Build and Run
```bash
./build.sh          # Compiles to raw 'calculator' executable
./calculator        # Run directly
# or
open calculator     # Launch via Finder/Dock
```

### Architecture

The application uses pure C with Objective-C runtime calls via `objc_msgSend`:

1. **No Bundle Required**: Compiles to a single executable with no `.app` bundle structure
2. **No Plist Required**: All app configuration happens in C code
3. **Programmatic UI**: Windows, menus, buttons created entirely via objc_msgSend
4. **Cocoa Integration**: Proper macOS app lifecycle and menu bar support

### Key Components

**Menu Setup** (calculator.c, lines 297-333):
- Creates NSMenu and NSMenuItem objects programmatically
- Dynamically reads app name from NSProcessInfo
- Sets up Quit menu item with Cmd+Q shortcut
- Calls `finishLaunching()` before `run()` for proper menu bar initialization

**UI Creation** (calculator.c, lines 335-397):
- Creates NSWindow with title, closable, and resizable styles
- Creates NSTextField for display
- Creates 16 calculator buttons in a 4x5 grid
- Wires button clicks to C callback function

**Calculator Logic** (calculator.c, lines 105-219):
- Pure C logic for arithmetic operations
- Maintains state: display, accumulator, operator, decimal tracking

### Known Quirks

**Initial Menu Visibility**: The menu bar doesn't appear until the app window comes to the foreground. This is a macOS behavior for raw executables without bundle metadata. Solution: Switch to another app and back to the calculator, or click on it in the Dock.

This matches the behavior of the minimal Objective-C experiment in the related `objc-gui-menu-without-bundle-or-plist-but-with-quit-menu-item` project.

### Files

- `calculator.c` - Main application code (pure C with Cocoa via objc_msgSend)
- `build.sh` - Simple build script
- `README.md` - User-facing documentation

### Technical Details

The application demonstrates:
- **Objective-C Runtime**: Using `objc_msgSend` for all Cocoa calls
- **Class Pair Creation**: Dynamically creating delegate classes for button clicks and window events
- **Memory Management**: Proper allocation and release of Objective-C objects
- **Platform-Specific ABI**: Handling ARM64 vs x86_64 calling conventions for msgSend

This proves that modern macOS app development doesn't require bundle/plist files for functional, integrated applications—they're conveniences for distribution and metadata, not technical requirements.
