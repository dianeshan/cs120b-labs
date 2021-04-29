/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1IAYFhXsAgymfE4eOy2lB1JXea0sF-RJy/view?usp=drivesdk
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

	if (x <= 0x26F/8) {
		tmpB = 0x01;
	}
	else if (x <= 0x26F/4) {
		tmpB = 0x03;
	}
	else if (x <= (0x26F/8)*3) {
		tmpB = 0x07;
	}
	else if (x <= 0x26F/2) {
		tmpB = 0x0F;
	}	
	else if (x <= (0x26F/8)*5) {
		tmpB = 0x1F;
	}
	else if (x <= (0x26F/8)*6) {
		tmpB = 0x3F;
	}
	else if (x <= (0x26F/8)*7) {
		tmpB = 0x7F;
	}
	else {
		tmpB = 0xFF;
	}

	PORTB = tmpB;
    }
    return 1;
}
