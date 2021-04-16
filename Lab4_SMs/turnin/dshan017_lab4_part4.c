/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #4  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum BT_States { BT_SMStart, BT_locked, BT_lhashp, BT_lhashr, BT_uhashp, BT_uhashr, BT_unlocked } BT_State;

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
			BT_State = BT_lhashp;
		}
		else {
			BT_State = BT_locked;
		}
		break;
	
	case BT_lhashp:
		if (tmpE == 0x00) {
			BT_State = BT_lhashr;
		} 
		else if (tmpC) {
			BT_State = BT_lhashp;
		}
		else if (tmpA || tmpB) {
			BT_State = BT_locked;
		}
		break;

	case BT_lhashr:
		if (tmpE == 0x00) {
			BT_State = BT_lhashr; 
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
		else if (tmpC) {
			BT_State = BT_uhashp;
		}
		else {
			BT_State = BT_unlocked;
		}
		break;

	case BT_uhashp:
		if (tmpE == 0x00) {
                        BT_State = BT_uhashr;
                }
                else if (tmpC) {
                        BT_State = BT_uhashp;
                }
                else if (tmpA || tmpB) {
                        BT_State = BT_unlocked;
                }
                break;

	case BT_uhashr:
		if (tmpE == 0x00) {
                        BT_State = BT_uhashr;
                }
                else if (tmpC || tmpA) {
                        BT_State = BT_unlocked;
                }
                else if (tmpB) {
                        BT_State = BT_locked;
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

	case BT_lhashp:
		break;

	case BT_lhashr:
		break; 

	case BT_uhashp:
                break;

	case BT_uhashr:
                break;
	case BT_unlocked:
		if (output == 0) {
			output = 1;
		}
		else {
			output = 0;
		}
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
