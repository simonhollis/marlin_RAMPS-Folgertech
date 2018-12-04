#ifndef I2C_EXPANDER_H

#define I2C_EXPANDER_H

#include "twibus.h"

char readExpanderValue(TWIBus i2c, char expander_adr_offset) ;
void writeExpanderValue(TWIBus i2c, char expander_adr_offset, char value) ;
void writeExpanderValueSafe(TWIBus i2c, char expander_adr_offset, char value) ;
void setExpanderDirections(char directions) ;

#endif
