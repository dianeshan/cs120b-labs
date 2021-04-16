/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum BT_States { BT_SMStart, BT_s0, BT_wait, BT_s1, BT_wait1 } BT_State;

void TickFct_Button() {
    unsigned char tmpA = PINA & 0x01;

    switch(BT_State) { //transitions
	case BT_SMStart: //initial transition
		BT_State = BT_s0;
		break;
	
	case BT_s0:
		if (tmpA) {
			BT_State = BT_s1;
		}	
		else {
			BT_State = BT_s0;
		}
		break;
	
	case BT_s1:
		if (tmpA) {
			BT_State = BT_s1;
		}
		else {
			BT_State = BT_wait1;
		} 
		break;

	case BT_wait1:
		if (tmpA) {
			BT_State = BT_wait; 
		}
		else {
			BT_State = BT_wait1;
		}
		break;

	case BT_wait:
		if (tmpA) {
			BT_State = BT_wait;
		}
		else {
			BT_State = BT_s0;
		}
		break;

	default:
		BT_State = BT_s0;
		break;
    }

    switch (BT_State) { //state actions
	case BT_s0:
		PORTB = 0x01;
		break;

	case BT_s1:
		PORTB = 0x02;
		break; 

	case BT_wait1:
		PORTB = 0x02;
		break;

	case BT_wait:
		PORTB = 0x01;
		break;	

	default:
		PORTB = 0x01;
		break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
    BT_State = BT_SMStart;
    
    while (1) {
	TickFct_Button();				
    }
    return 1;
}
