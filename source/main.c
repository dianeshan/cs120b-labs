/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:
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

unsigned short level1[] = {0, 0, 0, 1, 0, 0, 1, 0};
/*
unsigned short level1[] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0 };
unsigned short level2 = [ 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned short level3 = [ 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned short level4 = [ 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 1, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 0 ];
unsigned short level5 = [ 0, 0, 1, 0, 2, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 1, 0, 0, 0 ];
unsigned short level6 = [ 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 1, 0, 0, 0 ];
unsigned short level7 = [ 0, 0, 2, 0, 1, 0, 1, 0, 1, 0, 2, 0, 1, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 0, 1, 0, 0, 2, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 2, 0, 2, 0, 1, 0, 0, 1, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 0, 0, 2, 0, 2, 0, 0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 0, 1, 0, 0, 1, 0, 0, 2, 0, 1, 0, 0, 0 ];
*/

unsigned short i = 0;
unsigned char runpattern = 0;
unsigned char runrow = 0;
//unsigned char multip = ~PINA & 0x80;
//unsigned char singlep = ~PINA & 0x40;

enum run_states { run_wait, run_lvl1, run_lvl2, run_lvl3, run_lvl4, run_lvl5, run_lvl6, run_lvl7, run_fail, run_congrats, run_win };

//state machine for displaying hi welcome screen
enum open_states { open_wait, open_display1, open_display2, open_display3, open_display4, open_off };
//period of 1ms

unsigned char openpattern = 0x00;
unsigned char openrow = 0x00;

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
            openpattern = 0xFF; 
            openrow = 0x0E; //flat row top and bottom
            break;

        case open_display1:
            openpattern = 0x40;
            openrow = 0x11;
            break;

        case open_display2:
            openpattern = 0x20;
            openrow = 0x1B;
            break;

        case open_display3:
            openpattern = 0x10;
            openrow = 0x11;
            break;

        case open_display4:
            openpattern = 0x04;
            openrow = 0x11;
            break;

        case open_off:
            openpattern = 0x00;
            openrow = 0x00;
            break;

        default:
            break;
    }

    return state;
}

enum move_states { move_wait, move_start, move_norm, move_jump, move_duck };

unsigned char movepattern = 0x00;
unsigned char moverow = 0x00;

int move_tick(int state)
{
    unsigned char begin = ~PINA & 0x20;
    unsigned char jump = ~PINA & 0x01;
    unsigned char duck = ~PINA & 0x02;
    unsigned char jump2 = ~PINA & 0x04;
    unsigned char duck2 = ~PINA & 0x08;

    switch (state)
    {
    case move_wait:
        if (begin)
        {
            state = move_start;
        }
        else
        {
            state = move_wait;
        }
        break;

    case move_start:
	state = move_norm;
	break;

    case move_norm:
        if (jump || jump2)
        {
            state = move_jump;
        }
	else if (duck || duck2)
        {
            state = move_duck;
        }
        else
        {
            state = move_norm;
        }
        break;

    case move_jump:
        state = move_norm;
        break;

    case move_duck:
        state = move_norm;
        break;

    default:
        state = move_wait;
        break;
    }

    switch (state)
    {
    case move_wait:
	break;
    
    case move_start:
	moverow = 0x07;
        movepattern = 0x80;
        break;

    case move_norm:
	moverow = 0x07;
        if (movepattern == 0x01)
        { //reset to beginning of matrix
            movepattern = 0x80;
        }
        else
        {
            movepattern >>= 1;
        }
        break;

    case move_jump:
        moverow = 0x13;
        if (movepattern == 0x01)
        {
            movepattern = 0x80;
        }
        else
        {
            movepattern >>= 1;
        }
        break;

    case move_duck:
        moverow = 0x0F;
        if (movepattern == 0x01)
        {
            movepattern = 0x80;
        }
        else
        {
            movepattern >>= 1;
        }
        break;

    default:
        break;
    }

    return state;
}


int run_tick(int state) {
    unsigned char prev = 0;
    unsigned char numfails = 0;
    //unsigned short j = 0;
    //unsigned short k = 0;
    unsigned char begin = ~PINA & 0x20;

    unsigned char jump = ~PINA & 0x01;
    unsigned char duck = ~PINA & 0x02;
    unsigned char jump2 = ~PINA & 0x04;
    unsigned char duck2 = ~PINA & 0x08;

    switch (state)
    {
    case run_wait: //have a message displayed on matrix to show restarted or smth
        if (begin)
        {
            state = run_lvl1;
        }
        else
        {
            state = run_wait;
        }
        break;

    case run_lvl1:
        if (i < 8)
        {
            if (level1[i] == 1)
            {
                if (jump || jump2)
                {
                    i++;
                    state = run_lvl1;
                }
                else
                {
                    numfails = numfails + 1;
                    prev = 1; //to keep track of what level we are on
                    state = run_fail;
                }
            }
            else if (level1[i] == 2)
            {
                if (duck || duck2)
                {
                    i++;
                    state = run_lvl1;
                }
                else
                {
                    numfails = numfails + 1;
                    prev = 1; //to keep track of what level we are on
                    state = run_fail;
                }
            }
            else
            {
                i++;
                state = run_lvl1;
            }
        }
        else
        {
            i = 0;
            prev = 1;
            state = run_congrats;
        }
        break;

	case run_fail:
        if (numfails >= 3)
        {
            numfails = 0;
            state = run_wait;
        }
        else
        {
            if (prev == 1)
            {
		i = 0;
                state = run_lvl1;
            }
            else if (prev == 2)
            {
                state = run_lvl2;
            }
            else if (prev == 3)
            {
                state = run_lvl3;
            }
            else if (prev == 4)
            {
                state = run_lvl4;
            }
            else if (prev == 5)
            {
                state = run_lvl5;
            }
            else if (prev == 6)
            {
                state = run_lvl6;
            }
            else if (prev == 7)
            {
                state = run_lvl7;
            }
            else
            {
                state = run_wait;
            }
        }
        break;

    case run_congrats:
        if (prev == 1)
        {
            state = run_win;
        }
        else if (prev == 2)
        {
            state = run_lvl3;
        }
        else if (prev == 3)
        {
            state = run_lvl4;
        }
        else if (prev == 4)
        {
            state = run_lvl5;
        }
        else if (prev == 5)
        {
            state = run_lvl6;
        }
        else if (prev == 6)
        {
            state = run_lvl7;
        }
        else if (prev == 7)
        {
            state = run_win;
        }
        else
        {
            state = run_wait;
        }
        break;
    	
	case run_win: //display winning thing here
        	state = run_wait;
        	break;

	default:
            state = run_wait;
        	break;
	}

    switch (state)
    {
    case run_wait:
        //display will be from other state machine
        break;

    case run_lvl1:
        //go through level 1 array and display
        runpattern = 0x12;
	runrow = 0x1E;
        break;

	case run_fail:
        //display fail screen
        runpattern = 0xFF;
	runrow = 0x00;
	moverow = 0x1F;
	movepattern = 0x00;
	break;

    case run_congrats:
        //display pass level screen
        runpattern = 0xFF;
        runrow = 0x05;
	moverow = 0x1F;
	movepattern = 0x00;
        break;

    case run_win:
        //display you win screen
        runpattern = 0x55;
        runrow = 0x05;
	moverow = 0x1F;
	movepattern = 0x00;
        break;

    default:
        break;
    }

    return state;
}

enum display_states { display_display };


int display_tick (int state) {
	unsigned char finalpattern = 0x00;
	unsigned char finalrow = 0x00;
	
	switch (state) {
		case display_display:
			state = display_display;
			break;

		default:
			state = display_display;
			break;
	}

	switch (state) {
		case display_display:
			finalpattern = movepattern | openpattern | runpattern;
			finalrow = moverow | openrow | runrow;
			break;
	}

	PORTC = finalpattern;
	PORTD = finalrow;

	return state;
}

unsigned long int findGCD (unsigned long int a, unsigned long int b) {
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
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    /* Insert your solution below */

    static task task1, task2, task3, task4;
    task *tasks[] = { &task1, &task2, &task3, &task4 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
    
    const char start = -1;

    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &open_tick;    

    task2.state = start;
    task2.period = 300;
    task2.elapsedTime = task2.period;
    task2.TickFct = &move_tick;

    task3.state = start;
    task3.period = 2400;
    task3.elapsedTime = task3.period;
    task3.TickFct = &run_tick;    

    task4.state = start;
    task4.period = 1;
    task4.elapsedTime = task4.period;
    task4.TickFct = &display_tick;

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
