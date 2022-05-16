// USE_MOUSE has several methods which are not available to USE_STYLUS

#define USE_MOUSE
//#define USE_STYLUS

#include <absolute-mouse.h>

void setup() {
    AbsoluteMouse.begin();

    AbsoluteMouse.position(50,50);  // Move the cursor to screen center, but perform no action
    delay(1000);

    AbsoluteMouse.position(50, 30); // Move the cursor to horizontal center, 30% from top
    delay(1000);

    // With USE_MOUSE, several commands do not require explicit x and y arguments.
    // The command will be performed wherever the cursor is currently placed
    AbsoluteMouse.click();

    // Move the cursor to screen center, and scroll up by "1"
    // The distance scrolled depends on your device's mouse settings, so some trial and error is needed to find the correct value
    AbsoluteMouse.scroll(50, 50, -1);
    delay(1000);

    AbsoluteMouse.scroll(1);    //Scroll back down
}

void loop() {
    
}