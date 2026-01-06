// Test to debug menu rendering
#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include <string.h>

#ifdef __arm64__
#define abi_objc_msgSend_stret objc_msgSend
#define abi_objc_msgSend_fpret objc_msgSend
#else
#define abi_objc_msgSend_stret objc_msgSend_stret
#define abi_objc_msgSend_fpret objc_msgSend_fpret
#endif

typedef void NSApplication;
typedef void NSMenu;
typedef void NSMenuItem;
typedef void NSString;
typedef void NSWindow;
typedef void NSView;

typedef unsigned long NSUInteger;
typedef CGRect NSRect;

#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_id_id			((id (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_int		((void (*)(id, SEL, NSUInteger))objc_msgSend)
#define objc_msgSend_void_bool		((void (*)(id, SEL, BOOL))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)
#define objc_msgSend_uint			((NSUInteger (*)(id, SEL))objc_msgSend)
#define objc_msgSend_id_rect		((id (*)(id, SEL, NSRect))objc_msgSend)

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))

id cstring_to_nsstring(const char* cstr) {
	return ((id(*)(id, SEL, const char*))objc_msgSend)
		((id)objc_getClass("NSString"), 
		 sel_registerName("stringWithUTF8String:"), 
		 cstr);
}

int main() {
	NSApplication* app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	printf("Got NSApplication: %p\n", app);
	
	// Set activation policy FIRST
	objc_msgSend_void_int(app, sel_registerName("setActivationPolicy:"), 0);
	printf("Set activation policy\n");
	
	// Create menu before window
	id main_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	printf("Created main menu: %p\n", main_menu);
	
	id app_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	printf("Created app menu: %p\n", app_menu);
	
	// Create menu item with Quit
	id quit_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring("Quit"),
		 sel_registerName("terminate:"),
		 cstring_to_nsstring("q"));
	objc_msgSend_void_id(app_menu, sel_registerName("addItem:"), quit_item);
	printf("Added quit item\n");
	
	// Create app menu item - this is the one that shows in menu bar
	id app_menu_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring("App"),
		 NULL,
		 cstring_to_nsstring(""));
	objc_msgSend_void_id(app_menu_item, sel_registerName("setSubmenu:"), app_menu);
	objc_msgSend_void_id(main_menu, sel_registerName("addItem:"), app_menu_item);
	printf("Added app menu item with submenu\n");
	
	// Set main menu
	objc_msgSend_void_id(app, sel_registerName("setMainMenu:"), main_menu);
	printf("Set main menu\n");
	
	// Check menu count
	NSUInteger count = objc_msgSend_uint(main_menu, sel_registerName("numberOfItems"));
	printf("Main menu has %lu items\n", count);
	
	// Create a minimal window just to keep app alive
	NSRect frame = {{100, 100}, {200, 200}};
	NSWindow* window = ((id (*)(id, SEL, NSRect, NSUInteger, NSUInteger, BOOL))objc_msgSend)
		(NSAlloc(objc_getClass("NSWindow")), 
		 sel_registerName("initWithContentRect:styleMask:backing:defer:"), 
		 frame, 3, 2, 0);  // 3 = Titled|Closable, 2 = Buffered
	printf("Created window: %p\n", window);
	
	objc_msgSend_void_id(window, sel_registerName("setTitle:"), cstring_to_nsstring("Test"));
	objc_msgSend_void_bool(app, sel_registerName("activateIgnoringOtherApps:"), 1);
	((id(*)(id, SEL, id))objc_msgSend)(window, sel_registerName("makeKeyAndOrderFront:"), NULL);
	printf("Activated app and showed window\n");
	
	// Try to refresh menu bar
	objc_msgSend_void(app, sel_registerName("finishLaunching"));
	printf("Called finishLaunching\n");
	
	// Check if menu is still there
	id current_menu = objc_msgSend_id(app, sel_registerName("mainMenu"));
	printf("Current main menu: %p\n", current_menu);
	
	// Keep running for a moment
	int counter = 0;
	BOOL is_running = 1;
	while (is_running && counter < 5) {
		id pool = objc_msgSend_id(NSAlloc(objc_getClass("NSAutoreleasePool")), sel_registerName("init"));
		
		id event = ((id(*)(id, SEL, unsigned long long, void*, id, int))objc_msgSend)
			(app, sel_registerName("nextEventMatchingMask:untilDate:inMode:dequeue:"), 
			 ~0ULL, NULL, 
			 ((id(*)(id, SEL, const char*))objc_msgSend)((id)objc_getClass("NSString"), sel_registerName("stringWithUTF8String:"), "kCFRunLoopDefaultMode"),
			 1);
		
		if (event != NULL) {
			objc_msgSend_void_id(app, sel_registerName("sendEvent:"), event);
		}
		objc_msgSend_void(app, sel_registerName("updateWindows"));
		
		((void (*)(id, SEL))objc_msgSend)(pool, sel_registerName("release"));
		counter++;
	}
	
	printf("Done\n");
	return 0;
}
