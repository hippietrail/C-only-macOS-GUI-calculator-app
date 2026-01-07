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

**Menu Setup** (calculator.c, main function):
- Creates NSMenu and NSMenuItem objects programmatically
- Dynamically reads app name from NSProcessInfo
- Sets up Quit menu item with Cmd+Q shortcut
- Sets app delegate before finishLaunching

**App Delegate Callback** (calculator.c, `app_did_finish_launching`):
- Window and UI created **during** finishLaunching callback (not after)
- `activateIgnoringOtherApps:` called after window creation for immediate foreground rendering
- This ensures menu appears immediately without app-switching workaround

**Calculator Logic** (calculator.c, lines 105-220):
- Pure C logic for arithmetic operations  
- Maintains state: display, accumulator, operator, decimal tracking
- Supports `+`, `-`, `*`, `/` operations and decimal point input

### Critical Insight: App Delegate Timing

The key to making the menu appear immediately is **creating the window during the finishLaunching callback**, not after. The sequence is:

1. Set app delegate with `setDelegate:` 
2. Set main menu with `setMainMenu:`
3. Call `finishLaunching()` - triggers `applicationDidFinishLaunching:` callback
4. In the callback: create window, add UI, call `activateIgnoringOtherApps:`
5. After callback returns: call `run()` for event loop

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
