#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#include "i2c.h"

#define F_CPU 16000000UL //Define CPU clock Frequency 16MHz
#define SCL_CLK 100000L	 //Define SCL clock Frequency


void i2c_init(void) {	
	TWBR = (F_CPU / SCL_CLK - 16) / 2; // Set bit rate register = 72
	TWSR = (0<<TWPS0)|(0<<TWPS1); //TWPS: TWI Prescaler Bits 1
	TWCR = (1<<TWEN); // Enable TWI-interface and release TWI pins. 
}

void i2c_meaningful_status(uint8_t status) {
	switch (status) {
		case 0x08: // START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("START\n"));
			break;
		case 0x10: // repeated START transmitted, proceed to load SLA+W/R
			printf_P(PSTR("RESTART\n"));
			break;
		case 0x38: // NAK or DATA ARBITRATION LOST
			printf_P(PSTR("NOARB/NAK\n"));
			break;
		// MASTER TRANSMIT
		case 0x18: // SLA+W transmitted, ACK received
			printf_P(PSTR("MT SLA+W, ACK\n"));
			break;
		case 0x20: // SLA+W transmitted, NAK received
			printf_P(PSTR("MT SLA+W, NAK\n"));
				break;
		case 0x28: // DATA transmitted, ACK received
			printf_P(PSTR("MT DATA+W, ACK\n"));
			break;
		case 0x30: // DATA transmitted, NAK received
			printf_P(PSTR("MT DATA+W, NAK\n"));
			break;
		// MASTER RECEIVE
		case 0x40: // SLA+R transmitted, ACK received
			printf_P(PSTR("MR SLA+R, ACK\n"));
			break;
		case 0x48: // SLA+R transmitted, NAK received
			printf_P(PSTR("MR SLA+R, NAK\n"));
			break;
		case 0x50: // DATA received, ACK sent
			printf_P(PSTR("MR DATA+R, ACK\n"));
			break;
		case 0x58: // DATA received, NAK sent
			printf_P(PSTR("MR DATA+R, NAK\n"));
			break;
		default:
			printf_P(PSTR("N/A %02X\n"), status);
			break;
	}
}

inline void i2c_start() {
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Enable TWI, generate start TWSTA, clearing flag with TWINT, enables TWEN during star condition
    while (!(TWCR & (1<<TWINT))); // Waiting for transmission complete can be read from status register TWSR
									//// wait until transmission completed and until TWINT resets to zero
	//while((TWSR & 0xF8)!= 0x08); // Check for the acknowledgment (start transmitted)
}

inline void i2c_stop() {
	TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);	//Enable TWI, generate stop condition and clear interrupt flag 
	 while (TWCR & (1<<TWSTO)); //wait for stop bit to clear
}

inline uint8_t i2c_get_status(void) { 
	uint8_t status;
    status = TWSR & 0xF8; //mask status read the upper five bits from the TWSR status register, 
						//we mask out three lower bits. we can also detecting failures in I2C communication.
    return status;
}

inline void i2c_xmit_addr(uint8_t address, uint8_t rw) {
	TWDR = (address & 0xfe) | (rw & 0x01); // Load device address and R/W 
	TWCR = (1<<TWINT)|(1<<TWEN); //Enable TWI & clear interrupt flag
	while (!(TWCR & (1<<TWINT))); 
}


inline void i2c_xmit_byte(uint8_t data) {
	TWDR = data; 	// writes a data byte to TWDR register which is shifted to SDA line status can be read from status register TWSR.
	TWCR = (1<<TWINT)|(1<<TWEN); //Enable TWI & clear interrupt flag
    while (!(TWCR & (1<<TWINT))); 
}

inline uint8_t i2c_read_ACK() { //read byte with ACK
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); //Enable TWI, generation of ack clear interrupt flag
    while (!(TWCR & (1<<TWINT))); 
    return TWDR; //Return received data
}

inline uint8_t i2c_read_NAK() {	//read byte with NACK
	TWCR = (1<<TWINT)|(1<<TWEN); //Enable TWI and clear interrupt flag
	while (!(TWCR & (1<<TWINT))); 
    return TWDR;
}

inline void eeprom_wait_until_write_complete() { // veryfies with ACK that the writing to memory succefully
	char busy = 1;
	while(busy){
		i2c_start();
		i2c_xmit_addr(0xA0,0);

		if(i2c_get_status() == 0x18){
			busy = 0;
		}
	}
}


uint8_t eeprom_read_byte(uint8_t addr) {

	i2c_start();
	i2c_xmit_addr(0xA0,0);	
	i2c_xmit_byte(addr);
	
	i2c_start();
	i2c_xmit_addr(0xA0,1);

	i2c_read_NAK();	
	i2c_stop();

	return TWDR;	
}

void eeprom_write_byte(uint8_t addr, uint8_t data) {
	
	i2c_start();
	i2c_xmit_addr(0xA0,0);

	i2c_xmit_byte(addr);
	i2c_xmit_byte(data);
	
	i2c_stop();
	eeprom_wait_until_write_complete(); 	
}



void eeprom_write_page(uint8_t addr, uint8_t *data) {
	// ... (VG)
}

void eeprom_sequential_read(uint8_t *buf, uint8_t start_addr, uint8_t len) {
	// ... (VG)
}
