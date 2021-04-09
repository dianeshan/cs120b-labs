/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #3  Exercise #1
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
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x00; PORTB = 0xFF;
    DDRC = 0xFF; PORTC = 0x00; 
    /* Insert your solution below */
    unsigned char tmpA = 0x00;
    unsigned char tmpB = 0x00;
    unsigned char count;
    unsigned char result; 
    while (1) {
	tmpA = PINA;
	tmpB = PINB;
	count = 0;
	result = 0;
	while (count < 8) {
		result += (tmpA >> count) & 0x01;
		result += (tmpB >> count) & 0x01;
		count += 1;
	}		

	PORTC = result;
    }
    return 1;
}
