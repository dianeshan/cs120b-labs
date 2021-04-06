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
    DDRB = 0xFF; PORTB = 0x00; //Configure port B's 8 pins as outputs, initialize to 0's

    unsigned char tmpB = 0x00; //Temporary variable to hold the value of B
    unsigned char tmpA = 0x00; //Temporary variable to hold the value of A
    /* Insert your solution below */
    while (1) {
	// 1) Read input
	tmpA = PINA & 0x03;
	// 2) Perform computation
	// if PA0 is 1, set PB0 = 0, else = 1
	if (tmpA == 0x01) { // True if only PA0 is 1
		tmpB = 0x01 ; //sets tmpB to 00000001
	}
	else {
		tmpB = 0x00 ;  //Sets tmpB to 00000000
	}
	//3) Write Output	
	PORTB = tmpB;
    }
    return 0;
}
