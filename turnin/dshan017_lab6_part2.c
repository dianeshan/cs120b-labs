/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1M0ChoUuivQmJbpppn8SJem2bwwjBEiPD/view?usp=drivesdk
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

enum LightGame {s0, s1, s2, s3, s4, press, letgo} LG_State; 

void lightgame_tick() {
	
	unsigned char tmpA = ~PINA & 0x01;
	unsigned char tmpB = 0x00;

	switch (LG_State) {
		case s0:
			LG_State = s1;
			break;
		
		case s1:
			if (tmpA == 0x01) {
				tmpB = 0x01;
				LG_State = press;
			}
			else {
				LG_State = s2;
			}
			break;

		case s2:
			if (tmpA == 0x01) {
				tmpB = 0x02;
				LG_State = press;
			}
			else {
				LG_State = s3;
			}
			break;

		case s3:
			if (tmpA == 0x01) {
				tmpB = 0x04;
				LG_State = press;
			}
			else {
				LG_State = s4;
			}
			break;

		case s4:
			if (tmpA == 0x01) {
				tmpB = 0x02;
				LG_State = press;
			}
			else {
				LG_State = s1;
			}
			break;

		case press:
			if (tmpA == 0x01) {
				tmpB = PORTB;
				LG_State = press;
			}
			else {
				tmpB = PORTB;
				LG_State = letgo;
			}
			break;

		case letgo:
			if (tmpA == 0x01) {
				LG_State = s1;
			}
			else {
				tmpB = PORTB;
				LG_State = letgo;	
			}
			break;

		default:
			LG_State = s0;
	}

	switch (LG_State) {
		case s1:
			PORTB = 0x01;
			break;

		case s2:
			PORTB = 0x02;
			break;

		case s3:
			PORTB = 0x04;
			break;

		case s4:
			PORTB = 0x02;
			break;

		case press:
			PORTB = tmpB;
			break;

		case letgo:
			PORTB = tmpB;
			break;
		
		default:
			PORTB = 0x01;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(300);
    TimerOn();

    LG_State = s0;
    /* Insert your solution below */
    while (1) {
	lightgame_tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
