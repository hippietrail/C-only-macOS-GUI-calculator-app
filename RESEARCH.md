# macOS Menu Bar Rendering Issue - Research

## Problem
Menu is functionally working (Cmd+Q quits) but doesn't render visually in the menu bar.

## Investigation Results

### What We Know
1. Menu structure is correctly set up programmatically:
   - NSMenu and NSMenuItem objects created successfully
   - Menu items contain correct actions and key equivalents  
   - Menu properly set as app's main menu via `setMainMenu:`
   - The menu *functions* - Cmd+Q works, proving it's wired up

2. The issue is purely **visual rendering** in the menu bar

### Root Cause Analysis
macOS menu bar rendering is controlled by:
- **Dock/Finder integration**: Requires Info.plist in app bundle
- **Menu bar graphics rendering**: Handled by Windowserver/Aqua
- **Bundle validation**: System checks for proper .app bundle structure

The menu bar in macOS is rendered by a system service that:
1. Queries app bundles for Info.plist
2. Reads app name, icon, and menu configuration  
3. Renders in the top menu bar
4. Coordinates with Dock

### Why Pure C Executable Fails
A raw executable (not in .app bundle):
- Has no Info.plist
- Has no bundle identifier  
- Is not registered with Launchd/Finder
- Cannot participate in menu bar rendering

### Possible Solutions

#### Option 1: Create Minimal App Bundle
Create proper structure:
```
Calculator.app/
  Contents/
    MacOS/
      calculator (our executable)
    Info.plist (minimal)
    PkgInfo
```

This would allow the system to recognize and render the menu.

#### Option 2: Use NSApplication Methods
Try additional NSApplication initialization:
- `setApplicationIconImage:` 
- Setting bundle-like properties via runtime
- Force menu bar update via private APIs

#### Option 3: Accept Limitation  
The menu works, just doesn't appear in the menu bar visually.
Cmd+Q still quits, which is the functional requirement.

## Recommendation
Try Option 1 first - minimal .app bundle is the "right way" on macOS.
If that fails, Option 3 is acceptable - functionality is there.
