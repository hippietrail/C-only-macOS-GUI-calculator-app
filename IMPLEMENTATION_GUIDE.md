# macOS Calculator in Pure C - Implementation Guide

## Project Overview
A fully functional macOS calculator app written in pure C using Cocoa via `objc_msgSend` without any Objective-C language syntax.

**Status**: ✅ Complete with working menu bar

## Architecture

### Core Components

1. **Calculator State** (`calculator.c` lines 108-180)
   - Display value, accumulator, operator tracking
   - Arithmetic operation handler
   - Button click dispatcher

2. **Window & UI** (`calculator.c` lines 283-340)
   - 4x4 button grid (16 buttons)
   - NSTextField display
   - Window delegate for close handling

3. **Menu Bar** (`calculator.c` lines 262-290)
   - App menu with Quit option
   - Dynamic naming from NSProcessInfo
   - Cmd+Q keyboard shortcut

4. **Runtime Delegates** (`calculator.c` lines 224-232)
   - ButtonDelegate for button clicks
   - WindowDelegate for window close

### Key Design Decisions

#### No Objective-C Syntax
```c
// Instead of: [NSString stringWithUTF8String:@"Hello"]
// We use:
id nsstring = ((id(*)(id, SEL, const char*))objc_msgSend)
    ((id)objc_getClass("NSString"),
     sel_registerName("stringWithUTF8String:"),
     "Hello");
```

This demonstrates that Cocoa doesn't require Objective-C language features - the language is just syntax sugar over C and the Objective-C runtime.

#### App Bundle Structure
```
Calculator.app/
  Contents/
    MacOS/calculator        (compiled executable)
    Info.plist              (app metadata)
    PkgInfo                 (package type identifier)
```

Why this matters:
- Enables menu bar rendering by Windowserver/Aqua
- Allows proper Dock integration
- macOS requires bundle structure for visual menu bar display
- Raw executables can create menus programmatically but won't render them

#### Menu Bar Setup Order
1. Get NSApplication instance
2. Set activation policy to Regular (required macOS 10.6+)
3. Create NSMenu and NSMenuItem objects
4. Set as main menu BEFORE finishLaunching
5. Create window and show it
6. Call finishLaunching
7. Run event loop

## Building & Running

### Quick Build
```bash
./build.sh
open Calculator.app
```

### Manual Build
```bash
mkdir -p Calculator.app/Contents/MacOS
gcc -o Calculator.app/Contents/MacOS/calculator calculator.c \
    -framework Foundation -framework AppKit -lm
open Calculator.app
```

### Direct Execution (without bundle)
```bash
gcc -o calculator calculator.c -framework Foundation -framework AppKit -lm
./calculator
```
(Menu works but won't appear in menu bar without bundle)

## Technical Details

### objc_msgSend Casting
Every method call requires casting to specify:
- Return type
- Parameter types
- Calling convention (arm64 vs x86)

```c
#define objc_msgSend_id ((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void_id ((void (*)(id, SEL, id))objc_msgSend)
// ... many more for different signatures
```

This is necessary because objc_msgSend is varargs and compiler can't determine types.

### String Conversion
Cocoa uses NSString, C uses char*.

```c
id cstring_to_nsstring(const char* cstr) {
    return objc_msgSend_id_char_const(
        (id)objc_getClass("NSString"),
        sel_registerName("stringWithUTF8String:"),
        cstr);
}

const char* nsstring_to_cstring(id nsstr) {
    return objc_msgSend(nsstr, sel_registerName("UTF8String"));
}
```

### Dynamic Process Name
Instead of hardcoding "Calculator":
```c
const char* get_process_name(void) {
    id process_info = objc_msgSend_id(
        (id)objc_getClass("NSProcessInfo"),
        sel_registerName("processInfo"));
    id app_name = objc_msgSend_id(
        process_info,
        sel_registerName("processName"));
    return nsstring_to_cstring(app_name);
}
```

The app name comes from the bundle (Calculator.app) or executable name.

## Common Issues & Solutions

### Menu doesn't appear in menu bar
**Solution**: Use app bundle structure with Info.plist
```xml
<key>CFBundleName</key>
<string>Calculator</string>
<key>CFBundleExecutable</key>
<string>calculator</string>
```

### Menu exists but has no effect
**Solution**: Set activation policy before creating menu
```c
NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular);
```

### Window doesn't close when Cmd+Q pressed
**Solution**: Implement window delegate with `windowShouldClose:` method
```c
unsigned int window_should_close(void* self, SEL sel, id sender) {
    running = 0;
    return 1;
}
```

### Button clicks don't work
**Solution**: Set button target and action correctly
```c
objc_msgSend_void_id(button, sel_registerName("setTarget:"), delegate);
objc_msgSend_void_SEL(button, sel_registerName("setAction:"), 
                       sel_registerName("buttonClicked:"));
```

## Future Enhancements

### Decimal Point Support
Currently commented in button handler. Would need:
- Track decimal position
- Handle multiple decimals
- Format display correctly

### Additional Menu Items
- File menu (for future features)
- Edit menu (undo/redo)
- Help menu with about dialog

### UI Improvements
- Better button styling
- Font selection
- Window resizing behavior

## References

### Documentation Files
- `SOLUTION.md` - Menu bar rendering solution explanation
- `RESEARCH.md` - Investigation findings and patterns
- `PATTERNS.md` - Pure C Cocoa development approaches

### External References
- [Silicon Framework](https://github.com/EimaKve/Silicon) - Pure C Cocoa wrapper
  - See `examples/controls/menu.c` for complete menu example
- [RGFW](https://github.com/ColleagueRiley/RGFW) - Graphics/window framework

### Cocoa Documentation
- [NSApplication](https://developer.apple.com/documentation/appkit/nsapplication)
- [NSMenu & NSMenuItem](https://developer.apple.com/documentation/appkit/nsmenu)
- [NSWindow](https://developer.apple.com/documentation/appkit/nswindow)

## Learning Value

This project demonstrates:
1. ✅ Cocoa without Objective-C syntax
2. ✅ Runtime class creation and method registration
3. ✅ Proper macOS app bundle structure
4. ✅ Event handling and delegates
5. ✅ Menu bar integration
6. ✅ String marshaling between C and Objective-C

Perfect for understanding how macOS applications work at a fundamental level.
