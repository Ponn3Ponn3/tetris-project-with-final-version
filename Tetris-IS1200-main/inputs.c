#include <stdint.h>         
#include <pic32mx.h>      
#include <stdbool.h>  

int getbtns(void) {
	return ((PORTD & (7 << 5)) >> 4) | (PORTF >> 1 & 0x01);
}