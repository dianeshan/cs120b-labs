/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum BT_States { BT_SMStart, BT_locked, BT_hashp, BT_hashr, BT_unlocked } BT_State;

void TickFct_Button() {
    unsigned char tmpA = PINA & 0x01;
    unsigned char tmpB = PINA & 0x02;
    unsigned char tmpC = PINA & 0x04;
    unsigned char tmpD = PINA & 0x80;
    unsigned char tmpE = PINA;

    unsigned char output = 0x00;

    switch(BT_State) { //transitions
	case BT_SMStart: //initial transition
		BT_State = BT_locked;
		break;
	
	case BT_locked:
		if (tmpC) {
			BT_State = BT_hashp;
		}
		else {
			BT_State = BT_locked;
		}
		break;
	
	case BT_hashp:
		if (tmpE == 0x00) {
			BT_State = BT_hashr;
		} 
		else if (tmpC) {
			BT_State = BT_hashp;
		}
		else if (tmpA || tmpB) {
			BT_State = BT_locked;
		}
		break;

	case BT_hashr:
		if (tmpE == 0x00) {
			BT_State = BT_hashr; 
		}
		else if (tmpC || tmpA) {
			BT_State = BT_locked;
		}
		else if (tmpB) {
			BT_State = BT_unlocked;
		}
		break;

	case BT_unlocked:
		if (tmpD) {
			BT_State = BT_locked;
		}
		else {
			BT_State = BT_unlocked;
		}
		break;

	default:
		BT_State = BT_locked;
		break;
    }

    PORTC = BT_State;

    switch (BT_State) { //state actions
	case BT_locked:
		output = 0;
		break;

	case BT_hashp:
		break;

	case BT_hashr:
		break; 

	case BT_unlocked:
		output = 1;
		break;
	
	default:
		break;
    }

    PORTB = output;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */
    BT_State = BT_SMStart;
    
    while (1) {
	TickFct_Button();				
    }
    return 1;
}
