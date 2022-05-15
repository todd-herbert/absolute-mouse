#ifndef __ABSOLUTE_MOUSE_H__
#define __ABSOLUTE_MOUSE_H__

#include <Arduino.h>
#include "vusb/hid_descriptor.h"
#include "vusb/usbFunctionSetup.h"  //Define, once only, a required extern function of vusb, which we don't actually use

    
    #if defined(USE_MOUSE)
        #include "mouse/mouse.h"
        static MouseDevice AbsoluteMouse;

    #elif defined(USE_STYLUS)
        #include "stylus/stylus.h"
        static StylusDevice AbsoluteMouse;
    #else
        #error Please #define either USE_MOUSE or USE_STYLUS at the top of your program
    #endif

#endif