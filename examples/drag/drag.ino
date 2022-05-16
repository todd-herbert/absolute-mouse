#define USE_MOUSE
//#define USE_STYLUS

#include <absolute-mouse.h>

void setup() {
    AbsoluteMouse.begin();

    // Click and drag from near top left, to near bottom right, taking 2000ms
    AbsoluteMouse.drag(25, 25, -25, -25, 2000);
}

void loop() {
    
}