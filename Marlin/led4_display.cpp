#include "led4_display.h"

inline char getLEDValueFromASCII(char ascii){
  // Get value to write to LED controller PCF85741 LED segment register to represent value in ASCII passed as c
  switch (ascii){
    case '0':
      return 63 ;
    case '1':
      return 6 ;
    case '2':
      return 91 ;
    case '3':
      return 79 ;
    case '4':
      return 102 ;
    case '5':
      return 109 ;
    case '6':
      return 125 ;
    case '7':
      return 7 ;
    case '8':
      return 127 ;
    case '9':
      return 111 ;
    case '-':
      return 64 ;
    case '.': // decimal point
      return 128 ;
    case '*': // make a degree-like lifted 'o'
    	return 99 ;
    default:
    	return 128 ;
  }
}


void integerTo4Digits (int value, char digits[4]){
	// Take a value and chop it into the display values for the four digits of the display
	// Return in the int array

	// First calculate the int values for the digits. [1000s, 100s, 10s, 1s]
	digits[0] = value % 10 ;
	value -= digits[0] ;
	digits[1] = (value % 100) / 10 ;
	value -= digits[1] ;
	digits[2] = (value % 1000) / 100 ;
	value -= digits[2] ;
	if (value < 0) digits[3] = '-' ; // If negative, drop leading digit and display '-'
	else digits[3] = digits[3] = (value % 10000) / 1000 ;

	// Now convert them to ASCII, then the display digit segment integers
	for (int i = 0 ; i < 4 ; i++) {
		digits[i] = getLEDValueFromASCII(digits[i] + 48) ;
	}
}

void display4Digits(TWIBus i2c, char digits[4]){
	// Send the 4 digits on the I2C bus to a 4-digit display
	i2c.addr = 56 ;
	i2c.addbyte(0) ; // Address the control register
	i2c.addbyte(23) ; // Set display on at dimmest setting
	//i2c.addbyte(1) ; // Address the first digit
	i2c.addbytes(&digits[0], 4) ;
	i2c.send() ;
}

void displayTempOn4Digits(TWIBus i2c, Temperature thermalManager, int extruder){
	// Get extruder[extruder] temperature and display on the 4 digit display
	char digits[5] ; // one extra storage space to add in the degree symbol
	float temp_f = thermalManager.current_temperature[extruder] ;
	//int temp_i = static_cast<int>(temp_f) ; // Just discard floating bits. Could also std::floor std::ceil std::round (#include cmath)
	int temp_i = static_cast<int>(roundf(temp_f)) ;
	integerTo4Digits(temp_i, &digits[1]) ; // Pass the last 4 elements of array
	digits[0] = getLEDValueFromASCII('*') ; // Display degree symbol at right hand display
	display4Digits(i2c, digits) ; // Display, discarding 1000s place
}

void led_4display_update(TWIBus i2c, Temperature thermalManager, int active_extruder) {
	// Main function called in the Marlin_main.cpp idle() loop to update the display
	displayTempOn4Digits(i2c, thermalManager, active_extruder) ;
	return ;
}
