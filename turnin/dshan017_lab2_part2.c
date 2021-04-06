/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #1  Exercise #1
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

    unsigned char cntavail = 0x00; //Temporary variable to hold the value of B
    unsigned char tmpA = 0x00; //Temporary variable to hold the value of A
    /* Insert your solution below */
    while (1) {
	// 1) Read input
	tmpA = PINA & 0x0F;
	// 2) Perform computation
	// 
	if (tmpA == 0x01) { // True if only PA0 is 1
		cntavail = 0x03 ; //sets cntavail to 00000011
	}
	else if (tmpA == 0x02) {
		cntavail = 0x02 ;
	}
	else if (tmpA == 0x03) {
		cntavail = 0x01 ;
	}
	else if (tmpA == 0x04) {
		cntavail = 0x00 ;
	}
	else {
		cntavail = 0x04 ;  //Sets cntavail to 00000100
	}
	//3) Write Output	
	PORTC = cntavail;
    }
    return 0;
}
