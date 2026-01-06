// macOS Calculator in Pure C
// Compile with: gcc -o calculator calculator.c -framework Foundation -framework AppKit -lm

#include <objc/runtime.h>
#include <objc/message.h>
#include <CoreGraphics/CoreGraphics.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
typedef CGPoint NSPoint;
typedef CGSize NSSize;

typedef void NSApplication;
typedef void NSWindow;
typedef void NSView;
typedef void NSButton;
typedef void NSTextField;
typedef void NSEvent;
typedef void NSString;

#ifndef NSUInteger
typedef unsigned long NSUInteger;
typedef long NSInteger;
#endif

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

typedef NS_ENUM(NSUInteger, NSTextAlignment) {
	NSTextAlignmentRight = 2,
};

// objc_msgSend macros
#define objc_msgSend_id				((id (*)(id, SEL))objc_msgSend)
#define objc_msgSend_id_id			((id (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_id_rect		((id (*)(id, SEL, NSRect))objc_msgSend)
#define objc_msgSend_uint			((NSUInteger (*)(id, SEL))objc_msgSend)
#define objc_msgSend_int			((NSInteger (*)(id, SEL))objc_msgSend)
#define objc_msgSend_SEL			((SEL (*)(id, SEL))objc_msgSend)
#define objc_msgSend_float			((CGFloat (*)(id, SEL))abi_objc_msgSend_fpret)
#define objc_msgSend_bool			((BOOL (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void			((void (*)(id, SEL))objc_msgSend)
#define objc_msgSend_void_id		((void (*)(id, SEL, id))objc_msgSend)
#define objc_msgSend_void_uint		((void (*)(id, SEL, NSUInteger))objc_msgSend)
#define objc_msgSend_void_int		((void (*)(id, SEL, NSInteger))objc_msgSend)
#define objc_msgSend_void_bool		((void (*)(id, SEL, BOOL))objc_msgSend)
#define objc_msgSend_void_float		((void (*)(id, SEL, CGFloat))objc_msgSend)
#define objc_msgSend_void_double	((void (*)(id, SEL, double))objc_msgSend)
#define objc_msgSend_id_char_const	((id (*)(id, SEL, const char *))objc_msgSend)

#define NSAlloc(nsclass) objc_msgSend_id((id)nsclass, sel_registerName("alloc"))
#define NSRelease(obj) objc_msgSend_id((id)obj, sel_registerName("release"))

// ============================================================================
// String Utilities
// ============================================================================

id cstring_to_nsstring(const char* cstr) {
	return ((id(*)(id, SEL, const char*))objc_msgSend)
		((id)objc_getClass("NSString"), 
		 sel_registerName("stringWithUTF8String:"), 
		 cstr);
}

// ============================================================================
// Calculator State
// ============================================================================

struct {
	double display_value;
	double accumulator;
	char last_operator;
	BOOL new_number;
	NSTextField* display;
} calc_state = {0.0, 0.0, '\0', 1, NULL};

// Update display with current value
void update_display(double value) {
	char buffer[32];
	// Remove trailing zeros and decimal point if needed
	snprintf(buffer, sizeof(buffer), "%.10g", value);
	id ns_str = cstring_to_nsstring(buffer);
	objc_msgSend_void_id(calc_state.display, sel_registerName("setStringValue:"), ns_str);
}

// Perform arithmetic operation
double perform_operation(double lhs, char op, double rhs) {
	switch (op) {
		case '+': return lhs + rhs;
		case '-': return lhs - rhs;
		case '*': return lhs * rhs;
		case '/': return rhs != 0 ? lhs / rhs : 0;
		default: return rhs;
	}
}

// Handle number button press
void handle_number(const char* digit_str) {
	double digit = atof(digit_str);
	
	if (calc_state.new_number) {
		calc_state.display_value = digit;
		calc_state.new_number = 0;
	} else {
		// Append digit (simple approach: multiply and add)
		if (digit_str[0] == '.') {
			// TODO: Handle decimal point properly
			return;
		}
		calc_state.display_value = calc_state.display_value * 10 + digit;
	}
	
	update_display(calc_state.display_value);
}

// Handle operator button press
void handle_operator(char op) {
	// If we have a pending operator, execute it first
	if (calc_state.last_operator != '\0' && !calc_state.new_number) {
		calc_state.accumulator = perform_operation(
			calc_state.accumulator, 
			calc_state.last_operator, 
			calc_state.display_value
		);
		update_display(calc_state.accumulator);
	} else {
		calc_state.accumulator = calc_state.display_value;
	}
	
	calc_state.last_operator = op;
	calc_state.new_number = 1;
}

// Handle equals button press
void handle_equals(void) {
	if (calc_state.last_operator != '\0') {
		calc_state.display_value = perform_operation(
			calc_state.accumulator,
			calc_state.last_operator,
			calc_state.display_value
		);
		update_display(calc_state.display_value);
		calc_state.accumulator = 0;
		calc_state.last_operator = '\0';
		calc_state.new_number = 1;
	}
}

// ============================================================================
// Button Callbacks
// ============================================================================

#include <stdlib.h>

void button_clicked(void* self, SEL sel, id sender) {
	// Get button title
	id title_obj = ((id(*)(id, SEL))objc_msgSend)(sender, sel_registerName("title"));
	const char* title = ((const char*(*)(id, SEL))objc_msgSend)(title_obj, sel_registerName("UTF8String"));
	
	// Dispatch based on button type
	if (title[0] >= '0' && title[0] <= '9') {
		// Number button
		handle_number(title);
	} else if (title[0] == '.') {
		// Decimal point - TODO: implement properly
		printf("Decimal point not yet implemented\n");
	} else if (title[0] == '=') {
		// Equals
		handle_equals();
	} else if (title[0] == '+' || title[0] == '-' || title[0] == '*' || title[0] == '/') {
		// Operator
		handle_operator(title[0]);
	} else {
		printf("Unknown button: %s\n", title);
	}
}

// ============================================================================
// Main
// ============================================================================

int main(int argc, char* argv[]) {
	// Initialize app
	NSApplication* app = objc_msgSend_id((id)objc_getClass("NSApplication"), sel_registerName("sharedApplication"));
	objc_msgSend_void_int(app, sel_registerName("setActivationPolicy:"), 0); // NSApplicationActivationPolicyRegular
	
	// Create window
	NSRect frame = {{100, 100}, {320, 420}};
	NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;
	NSBackingStoreType backing = NSBackingStoreBuffered;
	
	SEL init_sel = sel_registerName("initWithContentRect:styleMask:backing:defer:");
	NSWindow* window = ((id (*)(id, SEL, NSRect, NSWindowStyleMask, NSBackingStoreType, BOOL))objc_msgSend)
		(NSAlloc(objc_getClass("NSWindow")), init_sel, frame, style, backing, 0);
	
	objc_msgSend_void_id(window, sel_registerName("setTitle:"), cstring_to_nsstring("Calculator"));
	objc_msgSend_void_bool(window, sel_registerName("setReleasedWhenClosed:"), 1);
	
	// Get content view
	NSView* content_view = objc_msgSend_id(window, sel_registerName("contentView"));
	
	// Create display (NSTextField)
	NSRect display_frame = {{10, 360}, {300, 40}};
	NSTextField* display = ((id (*)(id, SEL, NSRect))objc_msgSend)
		(NSAlloc(objc_getClass("NSTextField")), 
		 sel_registerName("initWithFrame:"), 
		 display_frame);
	
	objc_msgSend_void_id(display, sel_registerName("setStringValue:"), cstring_to_nsstring("0"));
	objc_msgSend_void_int(display, sel_registerName("setAlignment:"), NSTextAlignmentRight);
	objc_msgSend_void_bool(display, sel_registerName("setEditable:"), 0);
	objc_msgSend_void_id(content_view, sel_registerName("addSubview:"), display);
	
	calc_state.display = display;
	
	// Create button grid (4x5: 0-9, operators, equals)
	const char* button_labels[] = {
		"7", "8", "9", "/",
		"4", "5", "6", "*",
		"1", "2", "3", "-",
		"0", ".", "=", "+"
	};
	
	CGFloat btn_width = 70;
	CGFloat btn_height = 70;
	CGFloat margin = 5;
	CGFloat start_x = 10;
	CGFloat start_y = 20;
	
	for (int i = 0; i < 16; i++) {
		int row = i / 4;
		int col = i % 4;
		
		CGFloat x = start_x + col * (btn_width + margin);
		CGFloat y = start_y + row * (btn_height + margin);
		NSRect btn_frame = {{x, y}, {btn_width, btn_height}};
		
		NSButton* button = ((id (*)(id, SEL, NSRect))objc_msgSend)
			(NSAlloc(objc_getClass("NSButton")), 
			 sel_registerName("initWithFrame:"), 
			 btn_frame);
		
		objc_msgSend_void_id(button, sel_registerName("setTitle:"), cstring_to_nsstring(button_labels[i]));
		objc_msgSend_void_id(button, sel_registerName("setTarget:"), (id)&button_clicked);
		((void(*)(id, SEL, SEL))objc_msgSend)(button, sel_registerName("setAction:"), sel_registerName("button_clicked:"));
		
		objc_msgSend_void_id(content_view, sel_registerName("addSubview:"), button);
	}
	
	// Show window
	objc_msgSend_void_bool(app, sel_registerName("activateIgnoringOtherApps:"), 1);
	((id(*)(id, SEL, id))objc_msgSend)(window, sel_registerName("makeKeyAndOrderFront:"), NULL);
	
	// Run app
	objc_msgSend_void(app, sel_registerName("finishLaunching"));
	objc_msgSend_void(app, sel_registerName("run"));
	
	return 0;
}
