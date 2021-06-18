/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *	
 *	Demo Link: https://drive.google.com/file/d/1KRvwn4FbNBI38M-coTGadzs2S2UDSJzK/view?usp=drivesdk
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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

double seqOfNotes [] = { 293.66, 493.88, 440, 392, 293.66, 293.66, 293.66, 493.88, 440, 392, 329.63};
double noteTime [] = {300, 400, 300, 300, 500, 300, 300, 300, 300, 300, 400 };
unsigned char i = 0;
unsigned char playTime = 0;

enum BT_States { BT_Start, BT_Off, BT_Play, BT_Down, BT_Waitr } BT_State;


void bt_tick() {
    unsigned char tmpA = ~PINA & 0x01;
    unsigned char tick = 150;

    switch (BT_State) {
        case BT_Start:
            BT_State = BT_Off;
            break;

        case BT_Off:
            if (tmpA == 0x01) {
                BT_State = BT_Play;
            }
            else {
                BT_State = BT_Off;
            }
	    break;	    

        case BT_Play:
	    playTime = playTime + tick; 
            if (i >= 11) {
                if (tmpA == 0x01) {
                    BT_State = BT_Waitr;
                }
                else {
                    BT_State = BT_Off;
                }
            }
            else {
		if (playTime >= noteTime[i]) {
			playTime = 0;
			BT_State = BT_Down;
		}
		else {
			BT_State = BT_Play;
		}
            }
	    break;        

        case BT_Down:
	    i++;
            BT_State = BT_Play;
            break;

	case BT_Waitr:
	    if (tmpA == 0x01) {
		BT_State = BT_Waitr;
	    }
	    else {
		BT_State = BT_Off;
	    }
	    break;
        
        default:
            BT_State = BT_Start;
            break;
    }

    switch (BT_State) {
        case BT_Start:
            break;

        case BT_Off:
            set_PWM(0);
            i = 0;
	    playTime = 0;
            break;

        case BT_Play:
            set_PWM(seqOfNotes[i]);
            break;

        case BT_Down:
            set_PWM(0);
            break;

        case BT_Waitr:
            set_PWM(0);
            break;

        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(150);
    TimerOn();
    PWM_on();

    BT_State = BT_Start;

    while(1) {
    	bt_tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }	
    //PWM_off();
    return 1;
}
