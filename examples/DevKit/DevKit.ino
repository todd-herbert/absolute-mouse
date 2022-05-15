// DO NOT CONNECT 5V TO TARGET DEVICE
// See https://github.com/todd-herbert/absolute-mouse/blob/main/doc/self-powered/warning.md

/*========================================================================
                           ~~~~   DevKit  ~~~~                            

           MAKE SURE TO SET CONFIG BELOW BEFORE UPLOADING SKETCH

 This sketch is a tool for testing AbsoluteMouse commands and coordinates,
 without needing to rebuild over and over.

 Flash the sketch to your UNO and open a serial monitor (9600 baud) to
 access a commandline for executing AbsoluteMouse commands.

    _________________________________________________________________
    | >> click 5 10                                                  |
    | clicking at (5,10)                                             |
    |                                                                |
    | >> delay(1000); click(5,10);                                   |
    | pausing for 1000ms                                             |
    |                                                                |
    | clicking at (5,10)                                             |
    |                                                                |
    | >>                                                             |
    | _______________________________________________________________|


===============================  Config  =================================*/

// Should AbsoluteMouse emulate a Mouse or a Stylus?
// Both modes have their own benefits and limitations.
// Devices may respond best to one mode in particular; if you're having
// issues, try the other mode

        #define USE_MOUSE
        //#define USE_STYLUS


// AbsoluteMouse takes absolute coordinates, however it is up to you to
// define the system you will use, by provding the total width and height.
// By default, AbsoluteMouse expects "percentage" i.e. 100x100
// Consider setting to your screen dimension, either in mm or pixels
// You are free to use any value < 32768

// NOTE: IN NORMAL USE, DIMENSIONS ARE SET WITH EITHER 
// AbsoluteMouse.set_dimensions(width, height);
// Or during setup with AbsoluteMouse.begin(width, height);

        //#define SCREEN_WIDTH  100
        //#define SCREEN_HEIGHT 100

/*========================================================================*/



#define INPUT_MAX_LENGTH 500  // 25% of RAM on Arduino UNO

#include <Arduino.h>
#include "absolute-mouse.h"
#include "help.h"
#include "serial.h"

void setup() {

  Serial.begin(9600);

  Serial.println();
  Serial.println(F("=================================="));
  Serial.println(F("      absolute-mouse devkit       "));
  Serial.println(F("=================================="));
  Serial.println();
  Serial.println(F("type \"help\" for a list of commands"));
  Serial.println(F("type \"connect\" to begin a USB connection"));
  Serial.println();

}


char* serial_data = new char[INPUT_MAX_LENGTH];
bool connected = false;

void loop() {
  // Get raw command from serial
  // -----------------------------------------------------------------
  get_serial(serial_data, INPUT_MAX_LENGTH);


  // Split at semicolons - multiline commands
  char *line_splitter = serial_data;
  char *line;
  line = strtok_r(line_splitter, ";", &line_splitter);

  while (line != NULL) {  //For each command

    // Delimiter split the command and args
    // --------------------------------------------------------------
    char *command, *args[5];
    int arg_vals[5];
    char *arg_splitter = line;
    char *arg;

    // Split the command
    command = strtok_r(arg_splitter, " ,(", &arg_splitter);

    // Split the args
    int arg_count = 0;
      while (arg_count < 5)  {       // Still need args, and strtok will still provide them
        arg = strtok_r(arg_splitter, " ,(", &arg_splitter);
        if (arg == NULL) break;
        //else
        args[arg_count] = arg;
        arg_count++;
      }


    // Sanity check - argument count
    if ( arg_count > 5) {
      Serial.println(F("invalid args"));
      return;   // We'll try again next loop..
    }


    // Convert args to int
    if (strcmp(command, "help") != 0) { //String args for help, don't try to convert
      for(int i = 0; i < arg_count; i++)
        arg_vals[i] = atoi(args[i]);
    }


    // Interpret command
    // ---------------------------------------------------------

    if(strcmp(command, "connect") == 0) {
      #if defined(SCREEN_WIDTH) && defined(SCREEN_WIDTH)
        AbsoluteMouse.begin(SCREEN_WIDTH, SCREEN_HEIGHT);
      #else
        AbsoluteMouse.begin();
      #endif
      Serial.println(F("Beginning USB connection"));
      connected = true;
    }

    else if(strcmp(command, "help") == 0) {
      if(arg_count == 0)  print_help();
      else                print_help(args[0]);
    }

    else if (!connected) {
      Serial.println(F("First, type \"connect\" to begin a USB connection"));
      Serial.println();
      break;  //Time to get more serial input
    }
    else {

      if(strcmp(command, "delay") == 0) {
        if(arg_count == 1) {
          Serial.print(F("pausing for "));
          Serial.print(arg_vals[0]);
          Serial.println(F(" ms"));

          delay(arg_vals[0]);
        }
      }

      else if(strcmp(command, "set_dimensions") == 0) {
        if(arg_count == 2) {

          Serial.print(F("dimensions set to ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.set_dimensions(arg_vals[0], arg_vals[1]);
        }
      }

      else if (strcmp(command, "click") == 0) {
        if(arg_count == 2) {
          Serial.print(F("clicking at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.click(arg_vals[0], arg_vals[1]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 0) {
          Serial.println(F("clicking in place"));
          AbsoluteMouse.click();
        }
        #endif
      }

      else if (strcmp(command, "right_click") == 0) {
        if(arg_count == 2) {
          Serial.print(F("right clicking at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.right_click(arg_vals[0], arg_vals[1]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 0) {
          Serial.println(F("right clicking in place"));
          AbsoluteMouse.right_click();
        }
        #endif
      }

      else if (strcmp(command, "double_click") == 0) {
        if(arg_count == 2) {
          Serial.print(F("double clicking at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.double_click(arg_vals[0], arg_vals[1]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 0) {
          Serial.println(F("double clicking in place"));
          AbsoluteMouse.double_click();
        }
        #endif
      }

      else if (strcmp(command, "long_press") == 0) {
        if(arg_count == 3) {
          Serial.print(F("pressing at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.print(F(") for "));
          Serial.print(arg_vals[2]);
          Serial.println(F("ms"));

          AbsoluteMouse.long_press(arg_vals[0], arg_vals[1], arg_vals[2]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 1) {
          Serial.print(F("pressing in place for "));
          Serial.print(arg_vals[0]);
          Serial.println(F("ms"));
          AbsoluteMouse.long_press(arg_vals[0]);
        }
        #endif
      }

      else if (strcmp(command, "drag") == 0) {
        if(arg_count == 5) {
          Serial.print(F("dragging from ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.print(F(") to ("));
          Serial.print(arg_vals[2]);
          Serial.print(F(", "));
          Serial.print(arg_vals[3]);
          Serial.print(F("), duration "));
          Serial.print(arg_vals[4]);
          Serial.println(F("ms"));

          AbsoluteMouse.drag(arg_vals[0], arg_vals[1], arg_vals[2], arg_vals[3], arg_vals[4]);
        }
      }

      else if (strcmp(command, "hold") == 0) {
        if(arg_count == 2) {
          Serial.print(F("holding at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.hold(arg_vals[0], arg_vals[1]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 0) {
          Serial.println(F("clicking and holding in place"));
          AbsoluteMouse.hold();
        }
        #endif
      }

      else if (strcmp(command, "travel") == 0) {
        if(arg_count == 5) {
          Serial.print(F("traveling from ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.print(F(") to ("));
          Serial.print(arg_vals[2]);
          Serial.print(F(", "));
          Serial.print(arg_vals[3]);
          Serial.print(F("), duration "));
          Serial.print(arg_vals[4]);
          Serial.println(F("ms"));

          AbsoluteMouse.travel(arg_vals[0], arg_vals[1], arg_vals[2], arg_vals[3], arg_vals[4]);
        }
      }

      else if (strcmp(command, "release") == 0) {
        if(arg_count == 0) {
          AbsoluteMouse.release();
          Serial.println(F("releasing"));
        }
      }

      // Commands that only apply in mouse mode
      #ifdef USE_MOUSE

      else if (strcmp(command, "position") == 0) {
        if(arg_count == 2) {
          Serial.print(F("positioned at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.position(arg_vals[0], arg_vals[1]);
        }
      }

      else if (strcmp(command, "scroll") == 0) {
        if(arg_count == 3) {
          Serial.print(F("scrolling by "));
          Serial.print(arg_vals[2]);
          Serial.print(F(" with cursor at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.scroll(arg_vals[0], arg_vals[1], arg_vals[2]);
        }

        #ifdef USE_MOUSE
        if(arg_count == 1) {
          Serial.print(F("scrolling by "));
          Serial.print(arg_vals[0]);
          AbsoluteMouse.scroll(arg_vals[0]);
        }
        #endif
      }

      else if (strcmp(command, "middle_click") == 0) {
        if(arg_count == 2) {
          Serial.print(F("middle clicking at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.middle_click(arg_vals[0], arg_vals[1]);
        }
      }
      #endif

      // Commands that only apply in stylus mode
      #ifdef USE_STYLUS

      else if (strcmp(command, "blink") == 0) {
        if(arg_count == 2) {
          Serial.print(F("blinking the cursor at ("));
          Serial.print(arg_vals[0]);
          Serial.print(F(", "));
          Serial.print(arg_vals[1]);
          Serial.println(F(")"));

          AbsoluteMouse.blink(arg_vals[0], arg_vals[1]);
        }
      }

      #endif
    } // if (connected)

    Serial.println();
    line = strtok_r(line_splitter, ";", &line_splitter); // Load the next line incase multiline command
  }
}
