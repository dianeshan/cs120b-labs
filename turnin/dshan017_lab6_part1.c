/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1yyDKhljDV0r4dKM5KfdRFvRvdhXwnUuq/view?usp=drivesdk
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

enum Blink {s0, s1, s2, s3} B_State;

void blink_tick() {
	switch (B_State) {
		case s0:
			B_State = s1;
			break;
		
		case s1:
			B_State = s2;
			break;

		case s2:
			B_State = s3;
			break;

		case s3:
			B_State = s1;
			break;

		default:
			B_State = s0;
	}

	switch (B_State) {
		case s1:
			PORTB = 0x01;
			break;

		case s2:
			PORTB = 0x02;
			break;

		case s3:
			PORTB = 0x04;
			break;
		
		default:
			PORTB = 0x01;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    TimerSet(1000);
    TimerOn();

    B_State = s0;
    /* Insert your solution below */
    while (1) {
	blink_tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
