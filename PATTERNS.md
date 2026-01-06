# Pure C Cocoa Development Patterns

## Overview
Comparison of approaches for building macOS GUI apps in pure C using Cocoa.

## Pattern Comparison

### Direct objc_msgSend (Calculator App)
Most minimal, no abstraction layer.

**Pros:**
- Full control, understand every call
- Single file, easy to modify
- No framework dependencies beyond stdlib
- Teaches how Cocoa works under the hood

**Cons:**
- Verbose and repetitive
- Easy to get signatures wrong
- Need to cast every call
- More lines of code for same functionality

**Example:**
```c
id str = ((id(*)(id, SEL, const char*))objc_msgSend)
    ((id)objc_getClass("NSString"), 
     sel_registerName("stringWithUTF8String:"), 
     "Hello");
```

### Silicon Framework (Wrapper Approach)
Thin Cocoa wrapper library providing nicer C API.

**Pros:**
- Cleaner C API (no casting)
- Well-organized function names
- Handles common patterns (string conversion, process info)
- Still pure C, just one additional dependency

**Cons:**
- Another library to learn
- Less direct control
- Dependency on Silicon project

**Example:**
```c
NSMenuItem* item = NSMenuItem_init("Quit", selector(terminate), "q");
NSString_to_char(nsstring_value);
```

### Reference Patterns

#### Menu Bar Setup (Silicon pattern)
```c
// Get app name dynamically
const char* process_name = NSProcessInfo_processName(NSProcessInfo_processInfo());

// Create main menu
NSMenu* main_menu = NSAutoRelease(NSAlloc(SI_NS_CLASSES[NS_MENU_CODE]));
NSApplication_setMainMenu(NSApp, main_menu);

// Set activation policy (required on macOS 10.6+)
NSApplication_setActivationPolicy(NSApp, NSApplicationActivationPolicyRegular);
```

#### String Conversion Utilities
Both approaches need similar helpers:

**Direct:**
```c
id cstring_to_nsstring(const char* cstr) {
    return ((id(*)(id, SEL, const char*))objc_msgSend)
        ((id)objc_getClass("NSString"), 
         sel_registerName("stringWithUTF8String:"), 
         cstr);
}

const char* nsstring_to_cstring(id nsstr) {
    return ((const char*(*)(id, SEL))objc_msgSend)
        (nsstr, sel_registerName("UTF8String"));
}
```

**Silicon:**
```c
// Built-in NSString_to_char(nsstring)
```

#### Delegate Class Creation
Both use objc_allocateClassPair and objc_registerClassPair:

```c
Class delegate_class = objc_allocateClassPair(
    objc_getClass("NSObject"), "MyDelegate", 0);

class_addMethod(delegate_class, 
    sel_registerName("windowShouldClose:"), 
    (IMP)my_delegate_function, 
    "I@:@");  // signature

objc_registerClassPair(delegate_class);
```

## Key Insights

### macOS Requirements
1. **Activation Policy** (macOS 10.6+): 
   ```c
   NSApplication_setActivationPolicy(app, NSApplicationActivationPolicyRegular)
   ```
   Without this, menus won't appear even with proper setup.

2. **App Bundle Structure** (optional but recommended):
   ```
   App.app/Contents/MacOS/executable
   App.app/Contents/Info.plist
   App.app/Contents/PkgInfo
   ```
   Enables full Dock/Finder integration.

3. **NSProcessInfo for Dynamic Names**:
   ```c
   const char* app_name = NSProcessInfo_processName(NSProcessInfo_processInfo());
   ```
   Always use the app's actual name from system, don't hardcode.

### Choosing an Approach

**Use Direct objc_msgSend if:**
- Learning Cocoa internals
- Writing a small utility
- Want minimal dependencies
- Need to understand exactly what's happening

**Use a Framework (Silicon) if:**
- Building a production app
- Want cleaner code
- Need multiple windows/complex UI
- Working in a team

## Lessons from Reference Projects

### Silicon
- Demonstrates best practices for pure C Cocoa
- Shows proper abstraction without losing control
- Menu example is authoritative reference
- Worth studying for technique patterns

### RGFW
- Focuses on window/graphics layer
- No menu support (out of scope)
- Different design priorities (cross-platform)
- Good window/event handling reference

## Recommendation for Calculator

Current approach (direct objc_msgSend) is optimal because:
1. Educational value - shows how Cocoa actually works
2. Single file - easy to understand entire app
3. Minimal code - ~360 lines including UI
4. No dependencies - only system frameworks

If expanding to larger app, consider migrating to Silicon pattern.
