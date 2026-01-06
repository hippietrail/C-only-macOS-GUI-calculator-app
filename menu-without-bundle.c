// Minimal macOS App with Menu - No Bundle or Plist Required
// Compile with: gcc -o menu-demo menu-without-bundle.c -framework Foundation -framework AppKit

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include <string.h>

// ============================================================================
// Type Definitions & Macros
// ============================================================================

#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#define abi_objc_msgSend_fpret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#endif

typedef CGRect NSRect;
typedef void NSApplication;
typedef void NSWindow;
typedef void NSView;
typedef void NSString;

typedef unsigned long NSUInteger;
typedef long NSInteger;

#define NS_ENUM(type, name) type name; enum

typedef NS_ENUM(NSUInteger, NSWindowStyleMask) {
	NSWindowStyleMaskBorderless = 0,
	NSWindowStyleMaskTitled = 1 << 0,
	NSWindowStyleMaskClosable = 1 << 1,
	NSWindowStyleMaskMiniaturizable = 1 << 2,
	NSWindowStyleMaskResizable = 1 << 3,
};

typedef NS_ENUM(NSUInteger, NSBackingStoreType) {
	NSBackingStoreBuffered = 2
};

// objc_msgSend macros
#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_id_id			((id (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_id_rect		((id (*)(id, SEL, NSRect))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_bool		((void (*)(id, SEL, BOOL))objc_msgSend)
#define objc_msgSend_void_int		((void (*)(id, SEL, NSInteger))objc_msgSend)
#define objc_msgSend_id_char_const	((id (*)(id, SEL, const char *))objc_msgSend)

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))

// ============================================================================
// String Utilities
// ============================================================================

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

const char* get_process_name(void) {
	id process_info = objc_msgSend_id((id)objc_getClass("NSProcessInfo"), sel_registerName("processInfo"));
	id app_name = objc_msgSend_id(process_info, sel_registerName("processName"));
	return nsstring_to_cstring(app_name);
}

// ============================================================================
// Window Delegate
// ============================================================================

unsigned int window_should_close(void* self, SEL sel, id sender) {
	id app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	objc_msgSend_void_id(app, sel_registerName("terminate:"), NULL);
	return 1;
}

Class create_window_delegate_class(void) {
	Class delegate_class = objc_allocateClassPair(objc_getClass("NSObject"), "WindowDelegate", 0);
	class_addMethod(delegate_class, sel_registerName("windowShouldClose:"), (IMP)window_should_close, "I@:@");
	objc_registerClassPair(delegate_class);
	return delegate_class;
}

// ============================================================================
// App Delegate - Creates UI on finishLaunching
// ============================================================================

void app_did_finish_launching(void* self, SEL sel, id notification) {
	// Create window during finishLaunching callback
	NSRect frame = {{100, 100}, {400, 300}};
	NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
	NSBackingStoreType backing = NSBackingStoreBuffered;
	
	SEL init_sel = sel_registerName("initWithContentRect:styleMask:backing:defer:");
	id window = ((id (*)(id, SEL, NSRect, NSWindowStyleMask, NSBackingStoreType, BOOL))objc_msgSend)
		(NSAlloc(objc_getClass("NSWindow")), init_sel, frame, style, backing, 0);
	
	objc_msgSend_void_id(window, sel_registerName("setTitle:"), cstring_to_nsstring("Minimal App"));
	objc_msgSend_void_bool(window, sel_registerName("setReleasedWhenClosed:"), 1);
	
	// Set window delegate
	Class window_delegate_class = objc_getClass("WindowDelegate");
	id window_delegate = objc_msgSend_id(NSAlloc(window_delegate_class), sel_registerName("init"));
	objc_msgSend_void_id(window, sel_registerName("setDelegate:"), window_delegate);
	
	// Show window
	((id(*)(id, SEL, id))objc_msgSend)(window, sel_registerName("makeKeyAndOrderFront:"), NULL);
	
	// Bring app to foreground after window is created
	id app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	objc_msgSend_void_bool(app, sel_registerName("activateIgnoringOtherApps:"), 1);
}

Class create_app_delegate_class(void) {
	Class delegate_class = objc_allocateClassPair(objc_getClass("NSObject"), "AppDelegate", 0);
	class_addMethod(delegate_class, sel_registerName("applicationDidFinishLaunching:"), (IMP)app_did_finish_launching, "v@:@");
	objc_registerClassPair(delegate_class);
	return delegate_class;
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char* argv[]) {
	// Register delegate classes
	Class app_delegate_class = create_app_delegate_class();
	Class window_delegate_class = create_window_delegate_class();
	
	// Get app instance
	NSApplication* app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	
	// Set activation policy (make app appear in menu bar)
	objc_msgSend_void_int(app, sel_registerName("setActivationPolicy:"), 0); // NSApplicationActivationPolicyRegular
	
	// Set app delegate BEFORE finishLaunching so it gets the callback
	id app_delegate = objc_msgSend_id(NSAlloc(app_delegate_class), sel_registerName("init"));
	objc_msgSend_void_id(app, sel_registerName("setDelegate:"), app_delegate);
	
	// Create main menu
	id main_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	
	// Get app name from process info
	const char* app_name = get_process_name();
	char quit_title[256];
	snprintf(quit_title, sizeof(quit_title), "Quit %s", app_name);
	
	// Create app menu with Quit item
	id app_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	id quit_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring(quit_title),
		 sel_registerName("terminate:"),
		 cstring_to_nsstring("q"));
	objc_msgSend_void_id(app_menu, sel_registerName("addItem:"), quit_item);
	
	// Add app menu to menu bar
	id app_menu_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring(app_name),
		 NULL,
		 cstring_to_nsstring(""));
	objc_msgSend_void_id(app_menu_item, sel_registerName("setSubmenu:"), app_menu);
	objc_msgSend_void_id(main_menu, sel_registerName("addItem:"), app_menu_item);
	
	// Set main menu
	objc_msgSend_void_id(app, sel_registerName("setMainMenu:"), main_menu);
	
	// Finish launching - triggers applicationDidFinishLaunching callback
	objc_msgSend_void(app, sel_registerName("finishLaunching"));
	
	// Run event loop
	objc_msgSend_void(app, sel_registerName("run"));
	
	return 0;
}
