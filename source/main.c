/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct _task {
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
} task;

unsigned char level1 = [ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0 ];
unsigned char level2 = [ 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned char level3 = [ 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned char level4 = [ 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned char level5 = [ 0, 0, 1, 0, 2, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1, 0, 0, 0 ];
unsigned char level6 = [ 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0 ];
unsigned char level7 = [ 0, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 2, 0, 2, 0, 1, 0, 0, 1, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0 ];

unsigned short i = 0;
unsigned char pattern = 0;
unsigned char row = 0;
unsigned char begin = ~PINA & 0x20;
unsigned char multip = ~PINA & 0x80;
unsigned char singlep = ~PINA & 0x40;
unsigned char jump = ~PINA & 0x01;
unsigned char duck = ~PINA & 0x02;
unsigned char jump2 = ~PINA & 0x04;
unsigned char duck2 = ~PINA & 0x08;

enum run_states { run_wait, run_lvl1, run_lvl2, run_lvl3, run_lvl4, run_lvl5, run_lvl6, run_lvl7, run_fail, run_congrats, run_win };

//state machine for displaying hi welcome screen
enum open_states { open_wait, open_display1, open_display2, open_display3, open_display4, open_off };
//period of 1ms
int open_tick(int state) {
    unsigned char begin = ~PINA & 0x20;
    unsigned char game = ~PINA & 0x10;

    switch (state) {
        case open_wait:
            if (game) {
                state = open_display1;
            }
            else {  
                state = open_wait;
            }
            break;
        
        case open_display1:
            if (begin) {
                state = open_off;
            }
            else {
                state = open_display2;
            }
            break;
        
        case open_display2:
            if (begin)
            {
                state = open_off;
            }
            else
            {
                state = open_display3;
            }
            break;

        case open_display3:
            if (begin) 
            {
            state = open_off;
            }
            else
            {
                state = open_display4;
            }
            break;

        case open_display4:
            if (begin)
            {
                state = open_off;
            }
            else
            {
                state = open_display1;
            }
            break;

        case open_off:
            if (game) {
                state = open_display1;
            }
            else {
                state = open_off;
            }
            break;

        default:
            state = open_wait;
            break;
        }

    switch (state) {
        case open_wait:
            pattern = 0xFF; 
            row = 0x11; //flat row top and bottom
            break;

        case open_display1:
            pattern = 0x40;
            row = 0x0E;
            break;

        case open_display2:
            pattern = 0x20;
            row = 0x04;
            break;

        case open_display3:
            pattern = 0x10;
            row = 0x0E;
            break;

        case open_display4:
            pattern = 0x04;
            row = 0x0E;
            break;

        case open_off:
            pattern = 0x00;
            row = 0x00;
            break;

        default:
            break;
    }

    PORTC = pattern;
    PORTD = row;
    return state;
}

enum move_states { move_wait, move_norm, move_jump, move_duck };

int move_tick(int state)
{

    switch (state)
    {
    case move_wait:
        if (begin)
        {
            state = move_norm;
        }
        else
        {
            state = move_wait;
        }
        break;

    case move_norm:
        if (jump || jump2)
        {
            state = move_jump;
        }
        if (duck || duck2)
        {
            state = move_wait;
        }
        else
        {
            state = move_norm;
        }
        break;

    case move_jump:
        if (jump || jump2)
        {
            state = move_jump;
        }
        if (duck || duck2)
        {
            state = move_wait;
        }
        else
        {
            state = move_norm;
        }
        break;

    case move_duck:
        if (jump || jump2)
        {
            state = move_jump;
        }
        if (duck || duck2)
        {
            state = move_wait;
        }
        else
        {
            state = move_norm;
        }
        break;

    default:
        state = move_wait;
        break;
    }

    switch (state)
    {
    case move_wait:
        pattern = 0x80;
        row = 0x18;
        break;

    case move_norm:
        if (pattern == 0x01)
        { //reset to beginning of matrix
            pattern = 0x80;
        }
        else
        {
            pattern >>= 1;
        }
        break;

    case move_jump:
        row = 0x0C;
        if (pattern == 0x01)
        {
            pattern = 0x80;
        }
        else
        {
            pattern >>= 1;
        }
        break;

    case move_duck:
        row = 0x10;
        if (pattern == 0x01)
        {
            pattern = 0x80;
        }
        else
        {
            pattern >>= 1;
        }
        break;

    default:
        break;
    }

    PORTC = pattern;
    PORTD = row;
    return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    static task task1, task2;
    task *tasks[] = { &task1, &task2 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &open_tick;    

    task2.state = start;
    task2.period = 100;
    task2.elapsedTime = task2.period;
    task2.TickFct = &move_tick;

    //task3.state = start;
    //task3.period = 200;
    //task3.elapsedTime = task3.period;
    //task3.TickFct = &doorbellSMTick;    

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
    return 1;
}
