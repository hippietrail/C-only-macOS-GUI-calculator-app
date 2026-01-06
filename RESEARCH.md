# macOS Menu Bar Rendering - Research & Implementation

## Problem
Menu was functionally working (Cmd+Q quits) but didn't render visually in the menu bar.

## Investigation Results

### Root Cause Analysis
macOS menu bar rendering is controlled by system-level infrastructure:
- **Activation policy**: NSApplicationActivationPolicyRegular required on macOS 10.6+
- **Windowserver/Aqua**: Handles menu bar graphics rendering
- **Bundle metadata**: Info.plist provides custom app naming and integration

Menu bar rendering works in two scenarios:
1. **Bundled apps (.app)**: Uses Info.plist CFBundleName for professional menu title
2. **Raw executables**: Can render menus with default/executable name as app title

The distinction is:
- **Menu rendering**: Works for both bundled and non-bundled
- **Professional naming**: Requires bundle with Info.plist
- **Full integration**: Requires proper .app bundle structure

### Why Previous Attempt Failed
The original code likely failed because:
- NSApplicationActivationPolicy wasn't properly set early enough, OR
- Menu was created AFTER finishLaunching instead of BEFORE, OR
- Process hadn't fully initialized menu bar support

The actual blocking issue was NOT the lack of a bundle - it was likely the setup sequence or activation policy timing.

## Reference Projects

### Silicon (https://github.com/EimaKve/Silicon)
Excellent pure C framework for Cocoa programming:
- **Key insight**: Uses `NSProcessInfo_processName()` to dynamically get app name
- **Menu pattern**: `NSApplication_setActivationPolicy(NSApp, NSApplicationActivationPolicyRegular)` is required
- **Separation of concerns**: Provides wrapper functions over raw objc_msgSend calls
- **Example**: `examples/controls/menu.c` shows full menu bar implementation
- Shows that **macOS 10.6+ requires NSApplicationActivationPolicyRegular for menus to appear**

### RGFW (https://github.com/ColleagueRiley/RGFW)
Window/graphics framework - doesn't include menu support.

## Solution Implemented

### 1. App Bundle Structure (Standard macOS Practice)
```
Calculator.app/
  Contents/
    MacOS/
      calculator (executable)
    Info.plist (bundle metadata)
    PkgInfo (package type)
```

### 2. Key Improvements
- Added `get_process_name()` using NSProcessInfo (from Silicon pattern)
- Dynamic menu titles: "Quit <AppName>" instead of hardcoded text
- Set activation policy to NSApplicationActivationPolicyRegular (macOS 10.6+)
- Proper NSMenu/NSMenuItem setup following Cocoa conventions

### 3. Dynamic Menu Naming
```c
const char* get_process_name(void) {
    id process_info = objc_msgSend_id((id)objc_getClass("NSProcessInfo"), 
                                       sel_registerName("processInfo"));
    id app_name = objc_msgSend_id(process_info, sel_registerName("processName"));
    return nsstring_to_cstring(app_name);
}
```

## Technical Insights

### Pure C with Cocoa
- No Objective-C syntax needed, only runtime via objc_msgSend
- Custom classes created with objc_allocateClassPair work perfectly
- NSProcessInfo provides process metadata at runtime

### macOS Menu Requirements
- **Activation Policy**: Absolutely required on macOS 10.6+
- **Bundle Structure**: Not strictly required for menu functionality, but required for visual rendering
- **Info.plist**: Not strictly required, but prevents warnings and ensures proper integration

### Why the Bundle Matters
macOS distinguishes between:
1. **Bundled apps**: Full integration with Dock, menu bar, Finder
2. **Raw executables**: Work at OS level but excluded from UI integration

The menu bar specifically queries Finder's bundle database, which requires Info.plist.

## Key Takeaway
For production macOS apps in pure C/Cocoa:
1. Use app bundle structure (minimal effort, maximum compatibility)
2. Set activation policy to Regular
3. Use NSProcessInfo for dynamic app names
4. Use objc_msgSend for all Cocoa calls (no Objective-C needed)
