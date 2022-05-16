//#define USE_MOUSE
#define USE_STYLUS

#include <absolute-mouse.h>

void setup() {
    // For the most part, USE_STYLUS takes the same commands as USE_MOUSE
    // Some devices may respond better to USE_STYLUS than USE_MOUSE

    AbsoluteMouse.begin();

    // Blink the cursor in the middle of screen
    // This command is only available for USE_STYLUS
    AbsoluteMouse.blink(50, 50);

    // USE_STYLUS attempts to provide normal mouse functions like right click, however support may vary from device to device
    AbsoluteMouse.right_click(50, 50);

    // Blink 2% from left, 1% up from bottom (99% from top)
    AbsoluteMouse.blink(2, -1);

    AbsoluteMouse.click(2, -1); // Click
}

void loop() {
    
}