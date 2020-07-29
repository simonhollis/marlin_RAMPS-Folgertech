#include "led4_display.h"
//#include "ultralcd.h"

char getLEDValueFromASCII(char ascii){
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
    case '_':
      return 8 ;
    case '^': // Dash across top
      return 1 ;
    case '.': // decimal point
      return 128 ;
    case '*': // make a degree-like lifted 'o'
      return 99 ;
    case '#': // Light all segments and decimal point
      return 255 ;
    case ' ':
      return 0 ;
    case 'o':
      return 92 ;
    case 'O':
      return 63 ;
    case 'n':
      return 84 ;
    case 'N':
      return 84 ;
    case 'F':
      return 113 ;
    case 'f':
      return 113 ;
    case 'E':
      return 121 ;
    case 't':
      return 120 ;
    case 'Z':
      return 91 ;
    default:
      return 0 ;
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


static int last_temp = 0 ; // Store previously converted temperature

void displayTempOn4Digits(TWIBus i2c, Temperature thermalManager, int extruder){
	// Get extruder[extruder] temperature and display on the 4 digit display
	char digits[5] ; // one extra storage space to add in the degree symbol
	float temp_f = thermalManager.current_temperature[extruder] ;
	int temp_i = static_cast<int>(roundf(temp_f)) ;
	if (temp_i == last_temp) return ; // Don't need to do anything, so save CPU
	last_temp = temp_i ; // Otherwise, update with the new value
	integerTo4Digits(temp_i, &digits[1]) ; // Pass the last 4 elements of array
	digits[0] = getLEDValueFromASCII('*') ; // Display degree symbol at right hand display
	display4Digits(i2c, digits) ; // Display, discarding 1000s place
}

void led_4display_update(TWIBus i2c, Temperature thermalManager, int active_extruder) {
	// Main function called in the Marlin_main.cpp idle() loop to update the display
  static unsigned long prev_time = 0 ; // Previous time called
  unsigned long time = millis() ;
  if (time - prev_time > 500) { // Update max twice a second
    prev_time = time ;
	  displayTempOn4Digits(i2c, thermalManager, active_extruder) ;
  }
	return ;
}



// lcd_z_offset_edit()
// Call the zoffset menu:
// lcd_goto_screen(lcd_babystep_zoffset);