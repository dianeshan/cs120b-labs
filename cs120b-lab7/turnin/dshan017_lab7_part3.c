/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1VciJF4VrWl6umChpkmeNdiL3pf8HFhKi/view?usp=drivesdk
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF; 
    DDRB = 0xFF; PORTB = 0x00;
   //DDRD = 0xFF; PORTD = 0x00;

    /* Insert your solution below */
    ADC_init();

    while (1) {
	unsigned short x = ADC;
	unsigned char tmpB = 0x00;
	//unsigned char tmpD = (char) (x >> 8);

	if (x >= 0x26F/2) {
		tmpB = 0x01;
	}
	if (x < 0x26F/2) {
		tmpB = 0x00;
	}

	PORTB = tmpB;
    }
    return 1;
}
