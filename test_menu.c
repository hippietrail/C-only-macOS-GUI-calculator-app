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

typedef unsigned long NSUInteger;

#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_id_id			((id (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_int		((void (*)(id, SEL, NSUInteger))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))

id cstring_to_nsstring(const char* cstr) {
	return ((id(*)(id, SEL, const char*))objc_msgSend)
		((id)objc_getClass("NSString"), 
		 sel_registerName("stringWithUTF8String:"), 
		 cstr);
}

int main() {
	NSApplication* app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	objc_msgSend_void_int(app, sel_registerName("setActivationPolicy:"), 0);
	
	// Create menus
	id main_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	printf("Created main_menu: %p\n", main_menu);
	
	id app_menu = objc_msgSend_id(NSAlloc(objc_getClass("NSMenu")), sel_registerName("init"));
	printf("Created app_menu: %p\n", app_menu);
	
	// Create Quit item
	id quit_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring("Quit Calculator"),
		 sel_registerName("terminate:"),
		 cstring_to_nsstring("q"));
	printf("Created quit_item: %p\n", quit_item);
	
	objc_msgSend_void_id(app_menu, sel_registerName("addItem:"), quit_item);
	printf("Added quit_item to app_menu\n");
	
	// Create app menu item with title
	id app_menu_item = ((id(*)(id, SEL, id, SEL, id))objc_msgSend)
		(NSAlloc(objc_getClass("NSMenuItem")),
		 sel_registerName("initWithTitle:action:keyEquivalent:"),
		 cstring_to_nsstring("Calculator"),
		 NULL,
		 cstring_to_nsstring(""));
	printf("Created app_menu_item: %p\n", app_menu_item);
	
	objc_msgSend_void_id(app_menu_item, sel_registerName("setSubmenu:"), app_menu);
	printf("Set app_menu as submenu of app_menu_item\n");
	
	objc_msgSend_void_id(main_menu, sel_registerName("addItem:"), app_menu_item);
	printf("Added app_menu_item to main_menu\n");
	
	objc_msgSend_void_id(app, sel_registerName("setMainMenu:"), main_menu);
	printf("Set main_menu as app's main menu\n");
	
	objc_msgSend_void(app, sel_registerName("finishLaunching"));
	printf("Called finishLaunching\n");
	
	id current_menu = objc_msgSend_id(app, sel_registerName("mainMenu"));
	printf("Current main menu: %p (should be %p)\n", current_menu, main_menu);
	
	return 0;
}
