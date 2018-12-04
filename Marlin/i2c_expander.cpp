#include "i2c_expander.h"

static char io_directions[8] {255, 255, 255, 255, 255, 255, 255, 255} ; // Default to all pins input. Max 8 devices on a bus

// Read a byte value from PCF8574 I2C expander with address pin values set to 'expander_adr_offset'
char readExpanderValue(TWIBus i2c, char expander_adr_offset){
	char value ;
	i2c.addr = 32 + expander_adr_offset ;
	i2c.request(1) ; // Request a 1 byte read
	i2c.capture(&value, 1) ; // Copy receive buffer to 'value'
	return value ;
}

// Write a byte value 'value' to PCF8574 I2C expander with address pin values set to 'expander_adr_offset'
void writeExpanderValue(TWIBus i2c, char expander_adr_offset, char value){
	i2c.addr = 32 + expander_adr_offset ;
	i2c.addbyte(value) ;
	i2c.send() ;
}

// Write a byte value 'value' to PCF8574 I2C expander with address pin values set to 'expander_adr_offset'
// Make it safe by preventing any input port being driven by a value on its bit
void writeExpanderValueSafe(TWIBus i2c, char expander_adr_offset, char value){
	i2c.addr = 32 + expander_adr_offset ;
	value = value | io_directions[expander_adr_offset] ; // A '1' is safe for an input pin ; a '0' is not.
	i2c.addbyte(value) ;
	i2c.send() ;
}


// Set PCF8574 I2C expander pin directions. 'directions' is one bit per pin: 1 := input ; 0 := output
void setExpanderDirections(TWIBus i2c, char expander_adr_offset, char directions){
	char value ;
	io_directions[expander_adr_offset] = directions ;
	value = readExpanderValue(i2c, expander_adr_offset) ;
	writeExpanderValueSafe(i2c, expander_adr_offset, value) ;
}
