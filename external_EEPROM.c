#include <xc.h>
#include "main.h"


void write_EEPROM(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE_E);     //slave address
	i2c_write(address);    //particular address location if sec - 0x00 
	i2c_write(data);
	i2c_stop();
    
    for(int i=3000;i--;);
}

unsigned char read_EEPROM(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE_E);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ_E);
	data = i2c_read();
	i2c_stop();

	return data;
}