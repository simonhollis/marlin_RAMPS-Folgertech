#ifndef I2C_BUTTONS_H
#define I2C_BUTTONS_H

#include "twibus.h"

unsigned char i2c_read_buttons_common_with_leds(TWIBus i2c) ;
inline void i2c_write_leds_common_with_buttons(TWIBus i2c, unsigned char value) ;
void i2c_check_buttons(TWIBus i2c) ;

#endif
