#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>

#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "serial.h"
#include "timer.h"

void main (void) {

	i2c_init();
	uart_init();
	sei();
	
	
	uint8_t device_Address = 0x10;
	char my_name[7] = {"Douglas"};
	uint8_t read_byte_result;
	
	
	/*//Deluppgift_2_nr_2_DEMO. Testar eeprom_read_byte() 
	read_byte_result = eeprom_read_byte(device_Address);
	printf("%c", read_byte_result);*/

	
	
	//Deluppgift_2_nr_3_DEMO testar eeprom_write_byte() skriva in ditt förnamn på adress 0x10 
	//i EEPROMet, 
	for(int i = 0; i < sizeof(my_name)/sizeof(my_name[0]); i++) { //Get the sizeof the array
		eeprom_write_byte(device_Address, my_name[i]);
		device_Address ++; //move to the next adress
	}
	
	device_Address = 0x10; // resets the address to the start position
	
	//Deluppgift_2_nr_3_DEMO testar eeprom_read_byte()skriver ut det på UARTen
	for(int i = 0; i < sizeof(my_name)/sizeof(my_name[0]); i++) {
		read_byte_result = eeprom_read_byte(device_Address);
		printf("%c", read_byte_result);
		device_Address ++;
	}


	
	
	while (1) {
	
	}
}

