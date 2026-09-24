#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#if ARDUINO_USB_MODE != 0
#error Native USB OTG required
#endif
USBHIDKeyboard keyboard;
void setup(){keyboard.begin();USB.begin();}
void loop(){} // No keystrokes generated.
