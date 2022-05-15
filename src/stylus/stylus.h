/*
 * This file describes a digitizer / graphics tablet device, along with supporting methods.
 * The included VUSB library will perform the appropriate USB communications.
 */

#ifndef __STYLUS_H__
#define __STYLUS_H__

#include <Arduino.h>
#include "vusb/usbdrv.h"

class StylusDevice {
  //Methods
  public:
    
    // Pseudo constructors
    void begin(uint16_t width, uint16_t height);
    void begin();    //If unspecified, use percentage (100, 100)
    

    // Configure byte 1 of report
    void set_tip_switch     (bool state) { bitWrite(report[1], 0, state); }
    void set_barrel_switch  (bool state) { bitWrite(report[1], 1, state); }
    void set_eraser_switch  (bool state) { bitWrite(report[1], 3, state); }

    // High level methods
    void set_dimensions(uint16_t width, uint16_t height);
    void blink(int16_t x, int16_t y);
    void click(int16_t x, int16_t y);
    void right_click(int16_t x, int16_t y);
    void double_click(int16_t x, int16_t y);
    void long_press(int16_t x, int16_t y, uint16_t duration);
    void drag(int16_t from_x, int16_t from_y, int16_t to_x, int16_t to_y, uint16_t duration);
    void hold(int16_t x, int16_t y);
    void travel(int16_t to_x, int16_t to_y, int16_t from_x, int16_t from_y, uint16_t duration);
    void jump(int16_t to_x, int16_t to_y);
    void release();

  private:
    // Configure byte 1 of report (the bits that shouldn't be prodded by user)
    void set_invert         (bool state) { bitWrite(report[1], 2, state); }
    void set_in_range       (bool state) { bitWrite(report[1], 5, state); }

    void pos(uint16_t x, uint16_t y);                   // Bytes 2 - 5 of report, xy coords
    void update();                                      // Send stylus report
    int16_t encode_val(int16_t val, uint16_t op_edge);     // Encode coordinates for the HID report

    // Alternative delay method. Avoid Timer0 during critical USB functions
    void delay_ms(unsigned int ms) {
      for (unsigned int i = 0; i < ms; i++) {
        delayMicroseconds(1000);
      }
    }

  // Members
  public:
    uint8_t report[6] = {0x02, 0, 0, 0, 0, 0};  //Bit 0 is ReportID 2, to show that we're sending stylus data
  private:
    const uint8_t tx_delay = 20;
    uint16_t width = 100;
    uint16_t height = 100;
    
};

#endif //__ABSOLUTE_MOUSE_STYLUS_H__