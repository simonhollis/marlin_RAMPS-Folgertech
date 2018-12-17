#include "i2c_buttons.h"
#include "i2c_expander.h"
#include "Marlin.h" // For millis()

/* For momentary actions, consider using
 * built-in 'millis()' to get millisecond count
 *  and build a delay in repetition */

static unsigned char led_state = 0xff ; // Init to all LEDs off
/*
void button_disable_steppers(bool pressed){
	static bool steppers_active = true ;
	if (pressed){
		stepper.finish_and_disable() ;
		steppers_active = false ;
	}
	else if (!steppers_active){
//		  LCD_MESSAGEPGM(MSG_NO_MOVE);
		  enable_all_steppers();
		  steppers_active = true ;
	}
}

inline void button_bed_heat(bool pressed){
	static bool prev_pressed = true ; // Default to heat on
	if (!pressed && prev_pressed) {
		thermalManager.setTargetBed(0) ;
		prev_pressed = false ;
	}
	if (pressed) prev_pressed = true ;
}

void button_hold_for_z_offset(bool pressed){

}

inline void button_home(bool pressed){
	static bool prev_pressed = false ;
	if (pressed && !prev_pressed) {
		home_all_axes() ;
		prev_pressed = true ;
	}
	else if (!pressed) prev_pressed = false ;
}

inline void button_stop(bool pressed){
	static bool prev_pressed = false ;
	if (pressed && !prev_pressed) {
		gcode_M25() ; //M25: Pause SD Print
		// Also consider stop via LCD: gcode_M0_M1
		prev_pressed = true ;
	}
	if (!pressed && prev_pressed){
		gcode_M24() ; //M24: Start or Resume SD Print
		prev_pressed = false ;
	}
}
*/

#define I2C_BUTTON_DRIVE_PIN 7 // bit position of the N/O voltage sink terminal
#define I2C_BTNLEDS_OFFSET 0 // offset of the I2C expander on the I2C bus
#define I2C_NO_LED_BUTTONS 6 // How many LED buttons
#define I2C_BTNLEDS_MASK 0xc0 // What to always set to input
unsigned char i2c_read_buttons_common_with_leds(TWIBus i2c){
	// Read buttons, where the 'common' terminal of the button shares
	// a connection with the cathode of an LED, and the other
	// button pole is pulled up. Buttons share a common read terminal,
	// connected to I/O pin 'BUTTON_COMMON', and are scanned by driving
	// the individual button pins to ground (one button at a time)

	//// Temporarily turn off all LEDs and drive the button N/O terminal high for read mode (unnecessary)
	//writeExpanderValueSafe(i2c, I2C_BTNLEDS_OFFSET, 0xff) ;
	// Now, read the button values one by one. A button is pressed if we see '0' on
	// the common I/O pin. Mark this as a '1' in the 'pressed' variable
	unsigned char pressed = 0 ; 
	for (int i = 0 ; i < I2C_NO_LED_BUTTONS ; i++){
		unsigned char scan = 0xff & (~(0x01 << i)) ; // Select I/O #i to go low
		writeExpanderValueSafe(i2c, I2C_BTNLEDS_OFFSET, scan) ;
		unsigned char value = readExpanderValue(i2c, I2C_BTNLEDS_OFFSET) ;
		if ((value & 0x80) == 0) pressed = (pressed | (0x01 << i))  ; // '0' means pressed
	}
 
	// Now, re-drive the LEDs, setting the button N/O terminal to high impedance
	writeExpanderValueSafe(i2c, I2C_BTNLEDS_OFFSET, led_state | I2C_BTNLEDS_MASK) ;
	return pressed ;
}

inline void i2c_write_leds_common_with_buttons(TWIBus i2c, unsigned char value){
	led_state = value | I2C_BTNLEDS_MASK  ; // Mask off non-LED pins to high
	writeExpanderValueSafe(i2c, I2C_BTNLEDS_OFFSET, led_state) ;
}


void echoWord(unsigned char value) {
unsigned char digit1, digit2, digit3 ;
	// Convert to decimal before sending to the terminal
	// to prevent unprintable character problem on receiving end
	digit3 = value % 10 ;
	value -= digit3 ;
	digit2 = (value % 100) / 10 ;
	value -= digit2 ;
	digit1 = value / 100 ;
	SERIAL_CHAR(digit1 + 48) ;
	SERIAL_CHAR(digit2 + 48) ;
	SERIAL_CHAR(digit3 + 48) ;
	SERIAL_EOL() ;
}


void i2c_check_buttons(TWIBus i2c){
	static unsigned long prev_time = 0 ; // Previous time buttons were checked
	static bool initialised = false ;
	unsigned long time = millis() ;
	if (time - prev_time > 333) { // Check max 3 times a second
		prev_time = time ;
		if (initialised == false) {
			setExpanderDirections(i2c, I2C_BTNLEDS_OFFSET, I2C_BTNLEDS_MASK) ;
			initialised = true ;
		}
		// Check buttons
		//unsigned char pressed = readExpanderValue(i2c, I2C_BTNLEDS_OFFSET) ; // Flashes the LEDs :)
		unsigned char pressed = i2c_read_buttons_common_with_leds(i2c) ;
		i2c_write_leds_common_with_buttons(i2c, ~pressed) ; // Light only buttons that are pressed
    //echoWord(pressed) ;
	}
}
