/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "bit.h"
#include "timer.h"
#include "keypad.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct _task {
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
} task;

enum keypad_States { keypad_wait, keypad_press, keypad_release };

int keypadSMTick(int state) {
	unsigned char x = GetKeypadKey();
	
	switch (state) {
		case keypad_wait:
			if (x == '\0') {
				state = keypad_wait;
			}
			else {
				state = keypad_press;
			}	
			break;

		case keypad_press:
			state = keypad_release;
			break;

		case keypad_release:
			if (x == '\0') {
				state = keypad_wait;
			}
			else {
				state = keypad_release;
			}
			break;

		default:
			state = keypad_wait;
			break;
	}

	switch (state) {
		case keypad_wait:
			PORTB = 0x00;
			break;

		case keypad_press:
			PORTB = 0x80;
			break;

		case keypad_release:
			PORTB = 0x80;
			break;

		default:
			break;
	}

	return state;
}

unsigned long int findGCD( unsigned long int a, unsigned long int b) {
    unsigned long int c;
    while(1) {
        c = a%b;
        if (c == 0) { return b; }
        a = b;
        b = c;
    }

    return 0;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */

    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;    

    unsigned short i;

    unsigned long GCD = tasks[0]->period;
    for (i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while (1) {
	for (i = 0; i < numTasks; i++) {
            if (tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
