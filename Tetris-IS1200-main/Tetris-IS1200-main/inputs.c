#include <stdint.h>         
#include <pic32mx.h>      
#include <stdbool.h>  
/* code written by Pontus Rydén and Daniel Hartman 2022
*/

int getbtns(void) {
	return ((PORTD & (7 << 5)) >> 4) | (PORTF >> 1 & 0x01);
}