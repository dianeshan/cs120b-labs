/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #1  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; //Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00; //Configure port B's 8 pins as outputs, initialize to 0's

    unsigned char cntavail;
    unsigned char tmpA = 0x00; //Temporary variable to hold the value of A
    /* Insert your solution below */
    while (1) {
	// 1) Read input
	tmpA = PINA;
	cntavail = 0;
	// 2) Perform computation
	// 
	if (!(tmpA & 0x01)) {
		cntavail = cntavail + 1 ; //sets cntavail to +1
	}
	if (!((tmpA & 0x02) >> 1)) {
		cntavail = cntavail + 1 ;
	}
	if (!((tmpA & 0x04) >> 2)) {
		cntavail = cntavail + 1 ;
	}
	if (!((tmpA & 0x08) >> 3)) {
		cntavail = cntavail + 1 ;
	}
	if ((tmpA & 0x01) && ((tmpB & 0x02) >> 1) && ((tmpB & 0x04) >> 2) && ((tmpB & 0x08) >> 3)) {
		cntavail = 0x80;
	}
	//3) Write Output	
	PORTC = cntavail;
    }
    return 0;
}
