#ifndef  F_CPU
#define F_CPU 8000000L
#endif

#include <avr/io.h>
#include <util/twi.h>

#include "i2c.h"

#define TWI_FREQ	50000L   // TWI bit rate

void i2c_init() {
	// PORTC |= (1 << PC5);	// enable pull up on TWI clock line
	// PORTC |= (1 << PC4);	// enable pull up on TWI data line
	
	TWSR = 0;	// no prescaler
	TWBR = (F_CPU / TWI_FREQ - 16) / 2;	// set bit rate
}

uint8_t i2c_start(uint8_t address) {
    uint8_t twst;

    // send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // wait until transmission completed
    while((TWCR & (1<<TWINT)) == 0);

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ((twst != TW_START) && (twst != TW_REP_START)) return 1;

    // send device address
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // wail until transmission completed and ACK/NACK has been received
    while((TWCR & (1<<TWINT)) == 0);

    // check value of TWI Status Register. Mask prescaler bits.
    twst = TW_STATUS & 0xF8;
    if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) return 1;

    return 0;
}

uint8_t i2c_read_nack()
{
	// start TWI module without acknowledging reception
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// wait for end of transmission
	while((TWCR & (1<<TWINT)) == 0);
	
	// return received data from TWDR
	return TWDR;
}

uint8_t i2c_read_ack()
{
	// start TWI module and acknowledge data after reception
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	// wait for end of transmission
	while((TWCR & (1 << TWINT)) == 0);

	// return received data from TWDR
	return TWDR;
}

void i2c_start_wait(uint8_t address)
{
    uint8_t twst;

    while (1)
    {
	    // send START condition
	    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    
    	// wait until transmission completed
    	while((TWCR & (1<<TWINT)) == 0);
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ((twst != TW_START) && (twst != TW_REP_START)) continue;
    
    	// send device address
    	TWDR = address;
    	TWCR = (1 << TWINT) | (1 << TWEN);
    
    	// wail until transmission completed
    	while((TWCR & (1 << TWINT)) == 0);
    
    	// check value of TWI Status Register. Mask prescaler bits.
    	twst = TW_STATUS & 0xF8;
    	if ((twst == TW_MT_SLA_NACK ) || (twst == TW_MR_DATA_NACK)) 
    	{    	    
    	    /* device busy, send stop condition to terminate write operation */
	        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	        
	        // wait until stop condition is executed and bus released
	        while(TWCR & (1 << TWSTO));
	        
    	    continue;
    	}
    	break;
     }
}

uint8_t i2c_write(uint8_t data) {
	// load data into data register
	TWDR = data;
	
	// start transmission of data
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// wait for end of transmission
	while ((TWCR & (1 << TWINT)) == 0);
	
	// return status	
	return (TWSR & 0xF8) != TW_MT_DATA_ACK;
}

void i2c_stop() {
    /* send stop condition */
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    
    // wait until stop condition is executed and bus released
    while(TWCR & (1 << TWSTO));
}
