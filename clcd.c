#include<xc.h>
#include "main.h"

void clcd_write(unsigned char byte, unsigned char control_bit)
{
	CLCD_RS = control_bit;
    
	CLCD_PORT = byte;

	/* Should be atleast 200ns */
	CLCD_EN = HI;
	CLCD_EN = LO;
    
    //TRISD7 =1 (busy flag)
	PORT_DIR = INPUT;
    //read write bit (0-> write, 1 -> read)
	CLCD_RW = HI;
    
	CLCD_RS = INSTRUCTION_COMMAND;

	do
	{
		CLCD_EN = HI;
		CLCD_EN = LO;
	} while (CLCD_BUSY);

	CLCD_RW = LO;
    //iniitially D7 is input now we are making the D7 as output
	PORT_DIR = OUTPUT;
}

void init_clcd()
{
	/* Set PortD as output port for CLCD data */
	TRISD = 0x00;
	/* Set PortC as output port for CLCD control */
	TRISC = TRISC & 0xF8;

	CLCD_RW = LO;   //performing write operation

	
     /* Startup Time for the CLCD controller */
    __delay_ms(30); //giving the delay (>15ms) as per data sheet
    
    /* The CLCD Startup Sequence */
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INSTRUCTION_COMMAND	);
    __delay_us(1); 
    
    CURSOR_HOME;  //in order to make the cursor to be pointing at initial position
    __delay_us(100);
    TWO_LINE_5x8_MATRIX_8_BIT;
    __delay_us(100);
    CLEAR_DISP_SCREEN;
    __delay_us(500);
    DISP_ON_AND_CURSOR_OFF;
    __delay_us(100);
}

void clcd_print(const unsigned char *data, unsigned char addr)
{
    //instruction command is 0
	clcd_write(addr, INSTRUCTION_COMMAND);
    
    //upto null it will run 
	while (*data != '\0')
	{
        //to print whole string one by one
		clcd_write(*data++, DATA_COMMAND);
	}
}

void clcd_putch(const unsigned char data, unsigned char addr)
{
    //where RS = 0
	clcd_write(addr, INSTRUCTION_COMMAND);
    //what data (RS =1)
	clcd_write(data, DATA_COMMAND);
}
