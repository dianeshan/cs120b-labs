/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum BTSMStates {s0, wait, incp, decp, reset} BT_State; 

void button_tick() {
	
	unsigned char count = 0x00;
	unsigned char tmpA = ~PINA & 0x03;
	unsigned char tmpB = PORTB;

	switch (BT_State) {
		case s0:
			BT_State = wait;
			break;
		
		case wait:
			if (tmpA == 0x01) {
				BT_State = incp;
			}
			else if (tmpA == 0x02) {
				BT_State = decp;
			}
			else if (tmpA == 0x03) {
				BT_State = reset;
			}	
			else {
				BT_State = wait;
			}
			break;

		case incp:
			if (tmpA == 0x01) {
				BT_State = incp;
			}
			else if (tmpA == 0x03) {
				BT_State = reset;
			}
			else {
				BT_State = wait;
			}
			break;

		case decp:
			if (tmpA == 0x02) {
				BT_State = decp;
			}
			else if (tmpA == 0x03) {
				BT_State = reset;
			}
			else {
				BT_State = wait;
			}
			break;

		case reset:
			BT_State = wait;
			break;

		default:
			BT_State = wait;
			break;
	}

	switch (BT_State) {
		case s0:
			tmpB = 0x07;
			break;

		case wait:
			count = 0;
			break;

		case incp:
			if (tmpB < 9 && count == 0) {
				tmpB = tmpB + 1;
				count = 10;	
			}
			count -= 1;
			break;

		case decp:
			if (tmpB > 0 && count == 0) {
				tmpB = tmpB - 1;
				count = 10;
			}
			count -= 1;
			break;

		case reset:
			count = 10;
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
    TimerSet(100);
    TimerOn();

    BT_State = s0;
    /* Insert your solution below */
    while (1) {
	button_tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
