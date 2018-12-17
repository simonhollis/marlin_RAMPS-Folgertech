#ifndef I2C_EXPANDER_H

#define I2C_EXPANDER_H

#include "twibus.h"

unsigned char readExpanderValue(TWIBus i2c, unsigned char expander_adr_offset) ;
void writeExpanderValue(TWIBus i2c, unsigned char expander_adr_offset, unsigned char value) ;
void writeExpanderValueSafe(TWIBus i2c, unsigned char expander_adr_offset, unsigned char value) ;
void setExpanderDirections(TWIBus i2c, unsigned char expander_adr_offset, unsigned char directions) ;

#endif
