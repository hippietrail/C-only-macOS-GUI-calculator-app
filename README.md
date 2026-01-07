macOS Calculator GUI App in Pure C with no Bundle or plist

## Overview
A minimal working example calculator app built in pure C using Cocoa via `objc_msgSend` and Objective-C runtime, without using Objective-C language syntax or requiring a macOS Bundle.

## Features
- Calculator UI with 16 buttons (0-9, +, -, *, /, =)
- Display field showing results
- Working arithmetic operations (+, -, *, /)
- Native menu bar with Cmd+Q shortcut to quit
- No bundle or plist file required

## Building

### Quick Start (Recommended)
```bash
./build.sh
./calculator
```

### Manual Compilation
```bash
gcc -o calculator calculator.c -framework Foundation -framework AppKit -lm
./calculator
```

You can also launch via Finder by double-clicking the executable or using `open calculator`.

### Implementation Notes
- Uses `objc_msgSend()` for all runtime calls
- Custom delegate classes for window and button events
- No Objective-C language features, pure C with runtime introspection

Made with the help of [Amp Free](https://ampcode.com/free) from Sourcegraph and [Beads](https://github.com/steveyegge/beads) from Steve Yegge
