/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link:
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;

	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }

		else if (frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);

	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);

	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
} 

double seqOfNotes [] = { 293.66, 493.88, 440, 392, 293.66, 293.66, 293.66, 493.88, 440, 392, 329.63, 329.63 };
unsigned char noteTime [] = { 300, 300, 200, 300, 400, 200, 200, 300, 200, 200, 400, 200 };
unsigned char downTime [] = { 100, 100, 100, 200, 300, 100, 100, 200, 100, 200, 300, 0 };

unsigned char i = 0;

enum BT_States { BT_Start, BT_Off, BT_Note, BT_Down, BT_Buttonr } BT_State;

void bt_tick() {
    unsigned char tmpA = ~PINA & 0x01; //button input
    unsgined char tick = 100; //100 ms
    unsigned char playTime = 0; //note time array
    unsigned char pauseTime = 0; //down time array

    switch (BT_State) {
        case BT_Start:
            BT_State = BT_Off;
            break;

        case BT_Off:
            if (tmpA == 0x01) {
                BT_State = BT_Note;
            }
            else {
                BT_State = BT_Off;
            }
            break;

        case BT_Note:
            playTime = playTime + tick;
            if (downTime == 0 || i = 11) {
                if (tmpA == 0x01) {
                    BT_State = BT_Buttonr;
                }
                else {
                    BT_State = BT_Off;
                }
            }
            else if (playTime < noteTime[i]) {
                BT_State = BT_Note;
            }
            else if (playTime >= noteTime[i]) {
                playTime = noteTime[i] + playTime;
                BT_State = BT_Down;
            }
            else {
                playTime = 0;
                i++;
                BT_State = BT_Note;
            }
            break;

        case BT_Down:
            pauseTime = pauseTime + tick;
            if (pauseTime >= downTime[i]) {
                if (i >= 12) {
                    pauseTime = 0;
                    if (tmpA == 0x01) {
                        BT_State = BT_Buttonr;
                    }
                    else {
                        BT_State = BT_Off;
                    }
                }
                else {
                    pauseTime = 0;
                    i++;
                    BT_State = BT_Note;
                }
            }
            else {
                BT_State = BT_Down;
            }
            break;

        case BT_Buttonr:
            if (tmpA == 0x01) {
                BT_State = BT_Buttonr;
            }
            else {
                BT_State = BT_Off;
            }
            break;

    }

    switch (BT_State) {
        case BT_Start:
            break;

        case BT_Off:
            PWM_off();
            i = 0;
            playTime = 0;
            pauseTime = 0;
            break;
        
        case BT_Note:
            set_PWM(seqOfNotes[i]);
            break;

        case BT_Down:
            set_PWM(0);
            break;

        case BT_Buttonr:
            set_PWM(0);
            break;
 
        default:
            break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */

    TimerSet(100);
    TimerOn();
    PWM_on();
 
    BT_State = BT_Start;

    while(1) {
    	bt_tick();
    	while (!TimerFlag);
    	TimerFlag = 0;
    }	
    PWM_off();
    return 1;
}
