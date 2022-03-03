#include <pic32mx.h>
#include <stdint.h>

/* Parts of code are taken/inspired from mipslab.c written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall, Latest update 2017-04-21 by F Lundevall 

   Other parts are written by Pontus Rydén and Daniel Hartman 2022*/

int main(void) {
	TRISD |= 0xfe0;
	TRISF |= 0xfe0;

	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	
	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
        SPI2CON |= 0x60;
	
	/* Turn on SPI */
	SPI2CONSET = 0x8000;
	display_init();
	timer_init();

	IEC(0) = 0x100;
	IPC(0) = 0x1F;
	/*IEC(0) |= 0x8;
	IPC(3) = 0x1c00000;*/

  	enable_interrupt();

  	start_game();

	while(1)
        update(); // Update the screen

	return 0;
}


