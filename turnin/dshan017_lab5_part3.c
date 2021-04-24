/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #5  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum BT_States { BT_SMStart, BT_s0, BT_s1, BT_s2, BT_s3, BT_s4, BT_s5, BT_reset} BT_State;

void TickFct_Button() {
	unsigned char tmpA = ~PINA & 0x01;
	unsigned char tmpB = PORTB;

	switch (BT_State) { 
		case BT_SMStart:
			BT_State = BT_s0;
			break;

		case BT_s0:
			if (tmpA == 0x01) {
				BT_State = BT_s1;
			}
			else {
				BT_State = BT_s0;
			}
			break;

		case BT_s1:
			if (tmpA == 0x01) {
				BT_State = BT_s2;
			}
			else {
				BT_State = BT_s1;
			}
			break;

		case BT_s2:
			if (tmpA == 0x01) {
				BT_State = BT_s3;
			}
			else {
				BT_State = BT_s2;
			}
			break;

		case BT_s3:
			if (tmpA == 0x01) {
				BT_State = BT_s4;
			}
			else {
				BT_State = BT_s3;
			}
			break;

		case BT_s4:
			if (tmpA == 0x01) {
				BT_State = BT_s5;
			}
			else {
				BT_State = BT_s4;
			}
			break;

		case BT_s5:
			if (tmpA == 0x01) {
				BT_State = BT_reset;
			}
			else {
				BT_State = BT_s5;
			}
			break;

		case BT_reset:
			BT_State = BT_s0;
			break;

		default:
			BT_State = BT_s0;
			break;
	}

	switch (BT_State) {
		case BT_s0:
			tmpB = 0x3F;
			break;

		case BT_s1:
			tmpB = 0x03;
			break;

		case BT_s2:
			tmpB = 0x0F;
			break;

		case BT_s3:
			tmpB = 0x01;
			break;

		case BT_s4:
			tmpB = 0x10;
			break;

		case BT_s5:
			tmpB = 0x0D;
			break;

		case BT_reset:
			tmpB = 0x00;
			break;

		default: 
			break;
	}

	PORTB = tmpB;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x07;

    BT_State = BT_SMStart;
    /* Insert your solution below */
    while (1) {
    	TickFct_Button();
    }
    return 1;
}
