# Menu Bar Rendering - Debug Analysis

## Current Situation
- Menu is created programmatically (confirmed working in tests)
- Cmd+Q works (menu is wired to actions)
- **Menu is invisible in the menu bar** (user reported)

## Key Difference: Silicon vs Our Approach

### Silicon (Works):
```c
NSApplication_setActivationPolicy(NSApp, NSApplicationActivationPolicyRegular);
// ... create menus ...
NSApplication_run(NSApp);  // Uses built-in event loop
```

### Our Code (Menu invisible):
```c
NSApplication_setActivationPolicy(NSApp, NSApplicationActivationPolicyRegular);
// ... create menus ...
// Manual event loop using nextEventMatchingMask / sendEvent / updateWindows
```

## Hypothesis 1: Event Loop Difference

The manual event loop might not be updating the menu bar UI properly. Silicon's `NSApplication_run()` is a built-in loop that handles all the UI updates correctly.

**Test**: Try replacing manual loop with `NSApplication_run()`

## Hypothesis 2: Menu Bar Needs Explicit Update

After setting main menu, we might need to call:
```c
NSApplication_setMainMenu(app, main_menu);
objc_msgSend_void(app, sel_registerName("updateWindows"));  // Already doing this
// OR
objc_msgSend_void(app, sel_registerName("_updateAllWindowsUI")); // Private API?
```

## Hypothesis 3: Menu Must Be Created AFTER Window

Silicon creates the window AFTER the menus. We do:
1. Create menus
2. Create window
3. Show window

Silicon does:
1. Create menus
2. Set menus
3. finishLaunching? (not shown)
4. Create window

## Hypothesis 4: Bundle vs Executable Distinction

Raw executable just doesn't render menus no matter what, and:
- Bundle structure was a workaround, not a fix
- User's test shows menu still invisible even in bundle
- Real issue is the code/setup, not the app packaging

## Critical Questions

1. Does `./calculator` (raw executable) show menu? **User: NO**
2. Does `open Calculator.app` (bundled) show menu? **User: NO** 
3. What happens if we use `NSApplication_run()` instead of manual loop?
4. What if we move window creation to AFTER menu setup completes?

## What We Know For Sure

✅ Menu is created (objc_msgSend calls work)
✅ Menu is set as main menu (verified in tests)
✅ Activation policy is Regular
✅ Event system works (Cmd+Q works)

❌ Menu doesn't render in menu bar (visual issue only)

## Next Steps

1. **Replace manual event loop with NSApplication_run()**
   - Simplifies code
   - Uses Apple's tested loop
   - Might fix menu rendering

2. **Test window creation order**
   - Create window AFTER menu setup fully completes
   - See if order matters

3. **Add explicit menu bar update calls**
   - After setting main menu, explicitly tell app to update UI

4. **Compare byte-for-byte with Silicon**
   - Use Silicon's approach exactly
   - See if menu renders

## Code Change Proposal

Replace this:
```c
while (running) {
    id pool = objc_msgSend_id(NSAlloc(objc_getClass("NSAutoreleasePool")), sel_registerName("init"));
    id event = ((id(*)(id, SEL, unsigned long long, void*, id, int))objc_msgSend)
        (app, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"), 
         ~0ULL, NULL, 
         cstring_to_nsstring("kCFRunLoopDefaultMode"), 1);
    if (event != NULL) {
        objc_msgSend_void_id(app, sel_registerName("sendEvent:"), event);
    }
    objc_msgSend_void(app, sel_registerName("updateWindows"));
    NSRelease(pool);
}
```

With this:
```c
objc_msgSend_void(app, sel_registerName("run"));
```

This would match Silicon's approach exactly.
