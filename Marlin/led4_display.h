#ifndef LED4_DISPLAY_H
#define LED4_DISPLAY_H

#include "twibus.h"
#include "temperature.h"

inline char getLEDValueFromASCII(char ascii) ;

void integerTo4Digits (int value, char digits[4]) ;

void display4Digits(TWIBus i2c, char digits[4]) ;

void displayTempOn4Digits(TWIBus i2c, Temperature thermalManager, int extruder) ;

void led_4display_update(TWIBus i2c, Temperature thermalManager, int active_extruder) ;

#endif
