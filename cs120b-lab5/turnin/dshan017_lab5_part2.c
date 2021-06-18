/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #5  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum BT_States { BT_SMStart, BT_s0, BT_incp, BT_incr, BT_decp, BT_decr, BT_reset} BT_State;

void TickFct_Button() {
	unsigned char tmpA = ~PINA & 0x03;
	unsigned char tmpC = PORTC;

	switch (BT_State) { 
		case BT_SMStart:
			BT_State = BT_s0;
			break;

		case BT_s0:
			if (tmpA == 0x01) {
				BT_State = BT_incp;
			}
			else if (tmpA == 0x02) {
				BT_State = BT_decp;
			}
			else if (tmpA == 0x03) {
				BT_State = BT_reset;
			}
			break;

		case BT_incp:
			BT_State = BT_incr;
			break;

		case BT_incr:
			if (tmpA == 0x00) {
				BT_State = BT_s0;
			}
			else if (tmpA == 0x01) {
				BT_State = BT_incr;
			}
			else if (tmpA == 0x02) {
				BT_State = BT_decp;
			}
			else if (tmpA == 0x03) {
				BT_State = BT_reset;
			}
			break;

		case BT_decp:
			BT_State = BT_decr;
			break;

		case BT_decr:
			if (tmpA == 0x00) {
				BT_State = BT_s0;
			}
			else if (tmpA == 0x01) {
				BT_State = BT_decr;
			}
			else if (tmpA == 0x02) {
				BT_State = BT_incp;
			}
			else if (tmpA == 0x03) {
				BT_State = BT_reset;
			}
			break;

		case BT_reset:
			if (!tmpA) {
				BT_State = BT_s0;
			}
			else {
				BT_State = BT_reset;
			}
			break;

		default:
			BT_State = BT_s0;
			break;
	}

	switch (BT_State) {
		case BT_s0:
			break;

		case BT_incp:
			if (tmpC < 9) {
				tmpC = tmpC + 1;
			}
			break;

		case BT_incr:
			break;

		case BT_decp:
			if (tmpC > 0) {
				tmpC = tmpC - 1;
			}
			break;

		case BT_decr:
			break;

		case BT_reset:
			tmpC = 0x00;
			break;

		default: 
			break;
	}

	PORTC = tmpC;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x07;

    BT_State = BT_SMStart;
    /* Insert your solution below */
    while (1) {
    	TickFct_Button();
    }
    return 1;
}
