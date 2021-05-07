/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
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

double f[] = {261.63, 293.66, 329.63, 349.23, 392.0, 440.0, 493.88, 523.25 };
unsigned char i = 0;

enum DS_States { DS_Start, DS_Off, DS_Offr, DS_On, DS_Onr } DS_State;

void ds_tick() {
    unsigned char tmpA = ~PINA & 0x01;

    switch (DS_State) {
        case DS_Start:
            DS_State = DS_Off;
            break;

        case DS_Off:
            if (tmpA == 0x01) {
                DS_State = DS_On;
            }
            else {
                DS_State = DS_Off;
            }
            break;

	case DS_Offr:
	    if (tmpA == 0x01) {
		DS_State = DS_Offr;
	    }
	    else {
		DS_State = DS_Off;
	    }
	    break;

        case DS_On:
            if (tmpA == 0x01) {
                DS_State = DS_On;
            }
            else {
                DS_State = DS_Onr;
            }
            break;

	case DS_Onr:
	    if (tmpA == 0x01) {
		DS_State = DS_Offr;
	    }
	    else {
		DS_State = DS_Onr;
	    }
	    break;
    }

    switch (DS_State) {
        case DS_Start:
            break;

        case DS_Off:
            PWM_off();
            break;

        case DS_On:
            PWM_on();
            break;
        
        default:
            break;
    }

}

enum BT_States{ BT_Start, BT_Wait, BT_Inc, BT_Incr, BT_Dec, BT_Decr } BT_State;

void bt_tick() {
    unsigned char tmpB = ~PINA & 0x02;
    unsigned char tmpC = ~PINA & 0x04;

    switch (BT_State) {
        case BT_Start:
            BT_State = BT_Wait;
            break;

        case BT_Wait:
            if (tmpB == 0x02) {
                BT_State = BT_Inc;
            }
            else if (tmpC == 0x04) {
                BT_State = BT_Dec;
            }
            else {
                BT_State = BT_Wait;
            }
            break;

        case BT_Inc:
            BT_State = BT_Incr;
            break;

        case BT_Dec:
            BT_State = BT_Decr;
            break;

        case BT_Incr:
            if (tmpB == 0x02) {
                BT_State = BT_Incr;
            }
            else {
                BT_State = BT_Wait;
            }
            break;

        case BT_Decr:
            if (tmpC == 0x04) {
                BT_State = BT_Decr;
            }
            else {
                BT_State = BT_Wait;
            }
            break;

        default:
            BT_State = BT_Start;
            break;
    }

    switch (BT_State) {
        case BT_Start:
            break;

        case BT_Wait:
            break;

        case BT_Inc:
            if (i < 7) {
                i = i + 1;
            }
            break;

        case BT_Dec:
            if (i > 0) {
                i = i - 1;
            }
            break;

        case BT_Incr:
            break;

        case BT_Decr:
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
    BT_State = BT_Start;

    //PWM_off();

    while (1) {
	ds_tick();
	bt_tick();
	set_PWM(f[i]);
    }
    return 1;
}
