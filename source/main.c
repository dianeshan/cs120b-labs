/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #2  Exercise #4
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
    DDRB = 0x00; PORTB = 0xFF; //Configure port B's 8 pins as inputs
    DDRC = 0x00; PORTC = 0xFF; //Configure port C's 8 pins as inputs
    DDRD = 0xFF; PORTD = 0x00; //Configure port D's 8 pins as outputs, initialize to 0's

    unsigned char tmpA = 0x00; //Temporary variable to hold the value of A
    unsigned char tmpB = 0x00; //Temporary variable to hold the value of B
    unsigned char tmpC = 0x00; //Temporary variable to hold the value of C
    unsigned char result = 0x00;
    short totalWeight = 0x0000; 
    /* Insert your solution below */
    while (1) {
	// 1) Read input
	tmpA = PINA;
	tmpB = PINB;
	tmpC = PINC;
	// 2) Perform computation 
	if ((tmpA + tmpB + tmpC) > 0x8C) { // True if weight exceeds 140
		result  = result | 0x01; //sets PD0 to 1 (00000001)
	}
	if ((tmpA - tmpC) > 0x50 || (tmpC - tmpA) > 0x50) {
		result = result | 0x02; //sets PD1 to 1 (00000010)
	}
	totalWeight = (tmpA + tmpB + tmpC) >> 2;   	
	//3) Write Output
	PORTD = totalWeight | result;
    }
    return 0;
}
