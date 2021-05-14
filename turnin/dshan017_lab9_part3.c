/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/16XW2ouYiOuFajC5sc1AFfrEsStluLAIv/view?usp=drivesdk
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

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

unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;
unsigned char speakerSound = 0x00;
unsigned char period = 1;
unsigned long tl_time = 0;
unsigned long bl_time = 0;
unsigned char ss_time = 0;

enum ThreeLEDsSM { TL_Start, TL_s0, TL_s1, TL_s2  } TL_State;

void tl_tick() {
    switch (TL_State) {
        case TL_Start:
            TL_State = TL_s0;
            break;

        case TL_s0:
	    tl_time = tl_time + period;
            if (tl_time < 300) {
                TL_State = TL_s0;
            }
            else {
                tl_time = 0;
                TL_State = TL_s1;
            }
            break;

        case TL_s1:
	    tl_time = tl_time + period;
            if (tl_time < 300) {
                TL_State = TL_s1;
            }
            else {
                tl_time = 0;
                TL_State = TL_s2;
            }
            break;

        case TL_s2:
	    tl_time = tl_time + period;
            if (tl_time < 300) {
                TL_State = TL_s2;
            }
            else {
                tl_time = 0;
                TL_State = TL_s0;
            }
            break;

        default:
            break;
    }

    switch (TL_State) {
        case TL_Start:
            break;

        case TL_s0:
            threeLEDs = 0x01;
            break;

        case TL_s1:
            threeLEDs = 0x02;
            break;

        case TL_s2:
            threeLEDs = 0x04;
            break;

        default:
            break;
    }
}

enum BlinkingLEDSM { BL_Start, BL_On, BL_Off } BL_State;

void bl_tick() {
    switch (BL_State) {
        case BL_Start:
            BL_State = BL_On;
            break;

        case BL_On:
	    bl_time = bl_time + period;
            if (bl_time < 1000) {
                BL_State = BL_On;
            }
            else {
                bl_time = 0;
                BL_State = BL_Off;
            }
            break;

        case BL_Off:
	    bl_time = bl_time + period;
	    if (bl_time < 1000) {
                BL_State = BL_Off;
            }
            else {
                bl_time = 0;
                BL_State = BL_On;
            }
            break;

        default:
            BL_State = BL_Start;
            break;
    }

    switch (BL_State) {
        case BL_Start:
            break;

        case BL_On:
            blinkingLED = 0x08;
            break;

        case BL_Off:
            blinkingLED = 0x00;
            break;

        default:
            break;
    }
}

enum SpeakerSwitchSM { SS_Start, SS_Wait, SS_On, SS_Off } SS_State;

void ss_tick() {
    unsigned char tmpA = ~PINA & 0x04;

    switch (SS_State) {
	case SS_Start:
	     SS_State = SS_Wait;
	     break;

	case SS_Wait:
	     if (tmpA == 0x04) {
		SS_State = SS_On;
	     }
	     else {
		SS_State = SS_Wait;
	     }
	     break;

	case SS_On:
	     ss_time = ss_time + period;
	     if (ss_time < 2) {
	     	if (tmpA == 0x04) {
	 		SS_State = SS_On;
	     	}
	     	else {
			SS_State = SS_Wait;
	     	}
	     }
	     else {
		ss_time = 0;
		if (tmpA == 0x04) {
			SS_State = SS_Off;
		}
		else {
			SS_State = SS_Wait;
		}
	     }
	     break;

	case SS_Off:
	     ss_time = ss_time + period;
	     if (ss_time < 2) {
	     	if (tmpA == 0x04) {
			SS_State = SS_Off;
	     	}
	     	else {
			SS_State = SS_Wait;
	     	}
	     }
	     else {
		ss_time = 0;
		if (tmpA == 0x04) {
			SS_State = SS_On; 
		}
		else {
			SS_State = SS_Wait;
		}
	     }
	     break;

	default:
	     SS_State = SS_Start;
	     break;
    }

    switch (SS_State) {
	case SS_Start:
	     break;

	case SS_Wait:
	     speakerSound = 0x00;
	     break;

	case SS_On:
	     speakerSound = 0x10;
	     break;

	case SS_Off:
	     speakerSound = 0x00;
	     break;

	default:
	     break;
    }
}

unsigned char combinedLEDs = 0x00;

void cl_tick() {
    combinedLEDs = threeLEDs | blinkingLED | speakerSound;

    PORTB = combinedLEDs;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(1);
    TimerOn();

    /* Insert your solution below */
    TL_State = TL_Start;
    BL_State = BL_Start;
    SS_State = SS_Start;

    while (1) {
	tl_tick();
	bl_tick();
	ss_tick();
	cl_tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}
