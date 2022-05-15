#include "stylus/stylus.h"
#include "vusb/usbdrv.h"
#include "vusb/hid_descriptor.h"

extern PROGMEM const uint8_t usbHidReportDescriptor[];  // Scope the HID Report Descriptor into our class file

void StylusDevice::begin(uint16_t width, uint16_t height) {
  set_dimensions(width, height);
  begin();
}

// Begin the USB connection properly
void StylusDevice::begin() {

    PORTD = 0; // TODO: Only for USB pins?
    DDRD |= ~USBMASK;

    // Reconnect so device is detected
    cli();
    usbDeviceDisconnect();
    delay_ms(250);
    usbDeviceConnect();

    usbInit();
        
    sei();

    // Wait one second for enumeration to finish. (Android USB OTG requires >500ms)
    for(int i=0; i<1000; i++) {
        usbPoll();
        delay_ms(1);
    }    
}

// Bytes 2 - 5 of report, xy coords
void StylusDevice::pos(uint16_t x, uint16_t y) {
report[2] = x & 0xFF;
report[3] = (x >> 8) & 0x7F;
report[4] = y & 0xFF;
report[5] = (y >> 8) & 0x7F;
}

// Send the report (Tell host what our imaginary stylus is doing)
void StylusDevice::update() {
    usbPoll();
    if (usbInterruptIsReady()) {
        usbSetInterrupt(report, sizeof(report));
    }
}

// Encode the coordinate values for the HID report
int16_t StylusDevice::encode_val(int16_t val, uint16_t op_edge) {
  if (val < 0)  val = (op_edge + val) - 1;        // If negative, offset from opposite edge
  val = constrain((uint16_t)val, 0, op_edge);     // Clamp the value
  val = (((uint32_t)32767 * val) / op_edge);        // Scale to report val
  return val;
}

// Set the dimensions (outside of begin())
void StylusDevice::set_dimensions(uint16_t width, uint16_t height) {
  this->width = width;
  this->height = height;
}

// Display the cursor (for debugging)
void StylusDevice::blink(int16_t x, int16_t y) {
  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Show and hide cursor
  for (uint8_t blink = 0; blink < 3; blink++) {
    //Pen down
    set_in_range(true);

    for(uint16_t i = 0; i < 400; i+=tx_delay) {
      update();
      delay_ms(tx_delay);
    }

    //Pen up
    set_in_range(false);

    for(uint16_t i = 0; i < 400; i+=tx_delay) {
      update();
      delay_ms(tx_delay);
    }
  }
}

// Click at the specified position
void StylusDevice::click(int16_t x, int16_t y) {
  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Pen down
  set_in_range(true);
  set_tip_switch(true);
  update();

  //Pause for click to register
  delay_ms(tx_delay);

  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();
}

// Right click at the specified position
void StylusDevice::right_click(int16_t x, int16_t y) {
  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Press barrel switch button, and pen down
  set_in_range(true);
  set_tip_switch(true);
  set_barrel_switch(true);
  update();

  //Pause for click to register
  delay_ms(tx_delay);

  //Release stylus button, and pen up
  set_in_range(false);
  set_tip_switch(false);
  set_barrel_switch(false);
  update();
}

// Double click at the specified position
void StylusDevice::double_click(int16_t x, int16_t y) {
  //We could just call click() twice, but then it would recalculate all the values.
  //We'll do it the long way.

  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Pen down
  set_in_range(true);
  set_tip_switch(true);
  update();

  //Pause for click to register
  delay_ms(tx_delay);

  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();

  //Pause between clicks
  delay_ms(100);

  //Pen down, click number two
  set_in_range(true);
  set_tip_switch(true);
  update();

  //Pause for click to register
  delay_ms(tx_delay);

  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();
}

// Longpress at the specified position (mobile ui)
void StylusDevice::long_press(int16_t x, int16_t y, uint16_t duration) {
  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Pen down
  set_in_range(true);
  set_tip_switch(true);
  update();

  //Hold for the specified length
  for(uint16_t i = 0; i < duration; i += tx_delay)
    delay_ms(tx_delay);

  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();
}

// Perform a drag
void StylusDevice::drag(int16_t from_x, int16_t from_y, int16_t to_x, int16_t to_y, uint16_t duration) {
  //Check if duration is reasonable
  if(duration <= 100)  duration = 100;

  from_x = encode_val(from_x, width);
  from_y = encode_val(from_y, height);

  to_x = encode_val(to_x, width);
  to_y = encode_val(to_y, height);

  //Move the stylus to the start position
  pos(from_x, from_y);
  update();
  delay_ms(tx_delay);

  //Pen down
  set_in_range(true);
  set_tip_switch(true);
  update();
  delay_ms(tx_delay);

  //Move gradually towards end position
  uint16_t x = from_x;
  uint16_t y = from_y;
  
  uint32_t start = millis();
  uint32_t now;
  float progress = 0;
  do {
      now = millis();
      progress = (now - start) / (duration - 100.0);
      x = from_x + ((to_x - from_x) * progress);
      y = from_y + ((to_y - from_y) * progress);
      pos(x, y);
      update();
      delay_ms(tx_delay);
    } while (progress < 1);

  //Snap to end coords
  pos(to_x, to_y);
  update();
  delay_ms(tx_delay);


  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();
}

// Press and hold indefinitely
void StylusDevice::hold(int16_t x, int16_t y) {
  x = encode_val(x, width);
  y = encode_val(y, height);

  //Move the stylus to the position
  pos(x, y);
  update();
  delay_ms(tx_delay);

  //Pen down
  set_in_range(true);
  set_tip_switch(true);
  update();
}

// Travel (while already holding)
void StylusDevice::travel(int16_t to_x, int16_t to_y, int16_t from_x, int16_t from_y, uint16_t duration = 0) {
  //Check if duration is reasonable
  if(duration <= 100)  duration = 100;

  from_x = encode_val(from_x, width);
  from_y = encode_val(from_y, height);

  to_x = encode_val(to_x, width);
  to_y = encode_val(to_y, height);


  //Move the stylus to the start position
  pos(from_x, from_y);
  update();
  delay_ms(tx_delay);

  //Move gradually towards end position
  uint16_t x = from_x;
  uint16_t y = from_y;
  
  uint32_t start = millis();
  uint32_t now;
  float progress = 0;
  do {
      now = millis();
      progress = (now - start) / (duration - 100.0);
      x = from_x + ((to_x - from_x) * progress);
      y = from_y + ((to_y - from_y) * progress);
      pos(x, y);
      update();
      delay_ms(tx_delay);
    } while (progress < 1);

  //Snap to end coords
  pos(to_x, to_y);
  update();
  delay_ms(tx_delay);
}

// Release from StylusDevice::hold()
void StylusDevice::release() {
  //Pen up
  set_in_range(false);
  set_tip_switch(false);
  update();
}