/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1uUSOiuchVWS0clYxzYCAOpC2jKeRxq7N/view?usp=drivesdk
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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

void set_PWM(double frequency){
	static double current_frequency;

	if(frequency != current_frequency){
		if(!frequency){TCCR3B &= 0x08;}
		else{TCCR3B |= 0x03;}

		if(frequency < 0.954){OCR3A = 0xFFFF;}
		else if (frequency > 31250) {OCR3A = 0x0000;}
		else {OCR3A = (short) (8000000 / (128 * frequency)) - 1;}

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum keypad_State { keypad_wait, keypad_press, keypad_release, keypad_unlock };
char code[] = {'#', '1', '2', '3', '4', '5'};
unsigned char i = 0;

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
			if (x == code[i]) {
				if (i == 5) {
					state = keypad_unlock;
				}
				else {
					state = keypad_release;
				}
			}
			else {
				i = 0;
				state = keypad_wait;
			}
			break;

		case keypad_release:
			if (x == '\0') {
				++i;
				state = keypad_wait;
			}
			else {
				state = keypad_release;
			}
			break;

		case keypad_unlock:
			i = 0;
			state = keypad_wait;
			break;

		default:
			state = keypad_wait;
			break;
	}

	switch (state) {
		case keypad_wait:
			break;

		case keypad_press:
			break;

		case keypad_release:
			break;

		case keypad_unlock:
			PORTB = 0x01;
			break;

		default:
			break;
	}

	return state;
}

enum lock_States { lock_wait, lock_lock };

int lockSMTick(int state) {
	unsigned char tmpB = ~PINB & 0x80;	

	switch (state) {
		case lock_wait:
			if (tmpB) {
				state = lock_lock;
			}
			else {
				state = lock_wait;
			}
			break;

		case lock_lock:
			if (tmpB) {
				state = lock_lock;
			}
			else {
				state = lock_wait;
			}
			break;

		default:
			state = lock_wait;
			break;
	}

	switch (state) {
		case lock_wait:
			break;
	
		case lock_lock:
			PORTB = 0x00;
			break;
	}		

	return state;
}

double melody [] = { 493.88, 392, 440, 392, 293.66, 293.66, 440};
unsigned char j = 0;

enum doorbell_States { doorbell_start, doorbell_off, doorbell_play, doorbell_down, doorbell_waitr };

int doorbellSMTick(int state) {
	unsigned char tmpA = ~PINA & 0x80;

	switch (state) {
		case doorbell_start:
			state = doorbell_off;
			break;

		case doorbell_off:
			if (tmpA) {
				state = doorbell_play;
			}
			else {
				state = doorbell_off;
			}
			break;

		case doorbell_play:
			if (j >= 6) {
				if (tmpA) {
					state = doorbell_waitr;
				}
				else {
					state = doorbell_off;
				}
			}
			else {
				state = doorbell_down;
			}
			break;
			
		case doorbell_down:
			j++;
			state = doorbell_play;
			break;

		case doorbell_waitr:
			if (tmpA) {
				state = doorbell_waitr;
			}
			else {
				state = doorbell_off;
			}
			break;

		default:
			state = doorbell_off;
			break;
	}

	switch (state) {
		case doorbell_start:
			break;
	
		case doorbell_off:
			set_PWM(0);
			j = 0;
			break;

		case doorbell_play:
			set_PWM(melody[j]);
			break;

		case doorbell_down:
			set_PWM(0);
			break;

		case doorbell_waitr:
			set_PWM(0);
			break;

		default:
			break;
	}

	return state;
}

enum changecombo_States { changecombo_wait, changecombo_release, changecombo_insert, changecombo_insertrelease, changecombo_setcode };
char newcode[5];
unsigned char k = 0;

int changecomboSMTick (int state) {
    unsigned char x = GetKeypadKey();
    unsigned char tmpB = ~PINB & 0x80;

    switch (state) {
        case changecombo_wait:
            if (tmpB && (x == '*')) {
                state = changecombo_release;
            }
            else {
                state = changecombo_wait;
            }
            break;

        case changecombo_release:
	    PORTB = 0x02;
            if (x == '*') {
                state = changecombo_release;
            }
            else {
                state = changecombo_insert;
            }
            break;

        case changecombo_insert:
	    PORTB = 0x00;
            if (k == 6) {
                if (x == '#') { //designated end of combo
		    PORTB = 0x04;
		    state = changecombo_setcode;
                }
                else {
                    state = changecombo_insert;
                }
            }
            else {
                newcode[k] = x;
                state = changecombo_insertrelease;
            }
            break;

        case changecombo_insertrelease:
            if ((x == '\0') || (newcode[k - 1] == x)) {
                state = changecombo_insertrelease;
            }
            else {
		k++;
                state = changecombo_insert;
            }
            break;

        case changecombo_setcode:
	    PORTB = 0x00;
            state = changecombo_wait;
            break;

        default:
            state = changecombo_wait;
            break;
    }

    switch (state) {
        case changecombo_wait:
            k = 0;
            break;

        case changecombo_release:
            break;

        case changecombo_insert:
            break;

        case changecombo_insertrelease:
            break;
        
        case changecombo_setcode:
            for (int i = 0; i <= 5; i++) {
                code[i] = newcode[i];
            }
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
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0x7F; PORTB = 0x80;
    DDRC = 0xF0; PORTC = 0x0F;
    /* Insert your solution below */

    static task task1, task2, task3, task4;
    task *tasks[] = { &task1, &task2, &task3, &task4 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 10;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;    

    task2.state = start;
    task2.period = 10;
    task2.elapsedTime = task2.period;
    task2.TickFct = &lockSMTick;

    task3.state = start;
    task3.period = 200;
    task3.elapsedTime = task3.period;
    task3.TickFct = &doorbellSMTick;

    task4.state = start;
    task4.period = 100;
    task4.elapsedTime = task4.period;
    task4.TickFct = &changecomboSMTick;

    unsigned short i;

    unsigned long GCD = tasks[0]->period;
    for (i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();
    PWM_on();

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
    PWM_off();
    return 0;
}
