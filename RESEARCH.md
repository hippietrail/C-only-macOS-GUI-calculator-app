# macOS C GUI Research

## Conclusion: Use Cocoa via objc_msgSend

The best approach for a native macOS GUI in C is to use **Cocoa via Objective-C runtime functions**, specifically `objc_msgSend` and related runtime APIs.

## Key Findings

### Why Cocoa-in-Pure-C?
- Cocoa is the native modern macOS GUI framework
- You can call it entirely from C without writing Objective-C
- Uses C runtime functions: `objc_msgSend`, `objc_getClass`, `sel_registerName`, etc.
- Compiled with `-framework AppKit -framework Foundation` flags
- Two major projects use this: RGFW (single-header windowing) and Silicon.h (Cocoa wrapper)

### Architecture
1. **Type Setup**: Define C equivalents for Cocoa types (NSWindow, NSApplication, etc.) as void pointers
2. **Message Dispatch**: Use `objc_msgSend` with type-casted function pointers
3. **Macros**: Define helper macros for common objc_msgSend patterns to reduce boilerplate
4. **Callbacks**: Use `class_addMethod` to register custom delegate methods
5. **Event Loop**: Use NSApplication's event loop to process clicks, keyboard, etc.

### Compiler flags needed
```bash
gcc -lm -framework Foundation -framework AppKit -framework CoreVideo
```

### Key Functions
- `objc_msgSend` - Core runtime message dispatch
- `objc_getClass(name)` - Get class object by name
- `sel_registerName(name)` - Register/get method selector
- `class_addMethod()` - Add delegate callbacks
- `object_setInstanceVariable()` - Store custom data on objects

### Example: Button Click Flow
1. Register a delegate class with `objc_allocateClassPair`
2. Add button click handler with `class_addMethod`
3. Set the delegate on the button with `objc_msgSend_void_id`
4. When user clicks, objc runtime calls your C callback function

## For Our Calculator App
We'll need:
- NSApplication for the app instance
- NSWindow for the main window
- NSButton for number/operator buttons
- NSTextField or similar for display
- Delegate pattern for button callbacks
- String conversions between C and NSString

## References
- `research/Cocoa-in-Pure-C/example.c` - Full working example
- Apple's Objective-C runtime documentation
