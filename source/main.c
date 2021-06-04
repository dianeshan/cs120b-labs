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

typedef struct _task
{
    signed char state;
    unsigned long int period;
    unsigned long int elapsedTime;
    int (*TickFct)(int);
} task;

enum open_states
{
    open_wait,
    open_display1,
    open_display2,
    open_display3,
    open_display4,
    open_off
};
//period of 1ms

unsigned char openpattern = 0x00;
unsigned char openrow = 0x1F;

int open_tick(int state)
{
    unsigned char begin = ~PINA & 0x20;
    unsigned char game = ~PINA & 0x10;

    switch (state)
    {
    case open_wait:
        if (game)
        {
            state = open_display1;
        }
        else
        {
            state = open_wait;
        }
        break;

    case open_display1:
        if (begin)
        {
            state = open_off;
        }
        else
        {
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
        if (game)
        {
            state = open_display1;
        }
        else
        {
            state = open_off;
        }
        break;

    default:
        state = open_wait;
        break;
    }

    switch (state)
    {
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
        openrow = 0x1F;
        break;

    default:
        break;
    }

    return state;
}

unsigned char i = 0;
unsigned char runrow = 0x1F;
unsigned char runpattern = 0x00;
unsigned char moverow = 0x1F;
unsigned char movepattern = 0x00;
unsigned char lvl1pattern[] = {0, 0, 0, 16, 0, 0, 2, 0};
unsigned char lvl1row[] = { 31, 31, 31, 30, 31, 31, 30, 0};
char level1[] = {0, 0, 0, 1, 0, 0, 1, 0};

enum lvl1_states
{
    lvl1_wait,
    lvl1_start,
    lvl1_setup,
    lvl1_fail,
    lvl1_win,
};

int lvl1_tick(int state)
{

    unsigned char begin = ~PINA & 0x20;
    unsigned char jump = ~PINA & 0x01;
    unsigned char duck = ~PINA & 0x02;
    unsigned char jump2 = ~PINA & 0x04;
    unsigned char duck2 = ~PINA & 0x08;

    switch (state)
    {
    case lvl1_wait:
        if (begin) {
            state = lvl1_setup;
        }
        else {
            state = lvl1_wait;
        }
        break;

    case lvl1_setup:
        state = lvl1_start;
        break;

    case lvl1_start:
        if (i < 8) {
            if (level1[i] == 1)
            {
                if (jump || jump2)
                {
                    i++;
                    state = lvl1_start;
                }
                else
                {
                    i = 0;
                    state = lvl1_fail;
                }
            }
            else if (level1[i] == 2)
            {
                if (duck || duck2)
                {
                    i++;
                    state = lvl1_start;
                }
                else
                {
                    i = 0;
                    state = lvl1_fail;
                }
            }
            else
            {
                state = lvl1_start;
            }
        }
        else {
            state = lvl1_win;
        }
        break;

    case lvl1_fail:
        if (begin)
        {
            state = lvl1_setup;
        }
        else
        {
            state = lvl1_fail;
        }
        break;

    case lvl1_win:
        state = lvl1_wait;
        break;

    default:
        state = lvl1_wait;
        break;
    }

    switch (state) {
        case lvl1_wait:
            break;

        case lvl1_setup:
            runrow = 0x0F;
            runpattern = 0x12;
            break;

        case lvl1_start:
            moverow = 0x07;
            movepattern = 0x80;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
		moverow = 0x03;
            }
            else if (movepattern == 0x01 && (duck || duck2)) {
                movepattern = 0x80;
		moverow = 0x07;
            }
	    else if (movepattern == 0x01)
            {
                movepattern = 0x80;
            }
            else {
		moverow = 0x07;
		movepattern >>= 1;
            }
            break;

	    case lvl1_fail:
		runrow = 0x00;
		runpattern = 0xFF;
		break;

	   case lvl1_win:
		runrow = 0x05;
		runpattern = 0x55;
		break;

	   default:
		break;
        }

        return state;
}

enum display_states
{
    display_display
};

int display_tick(int state)
{
    unsigned char finalpattern = 0x00;
    unsigned char finalrow = 0x00;

    switch (state)
    {
    case display_display:
        state = display_display;
        break;

    default:
        state = display_display;
        break;
    }

    switch (state)
    {
    case display_display:
        finalpattern = runpattern | openpattern | movepattern;
        finalrow = runrow & openrow & moverow;
        break;
    }

    PORTC = finalpattern;
    PORTD = finalrow;

    return state;
}

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while (1)
    {
        c = a % b;
        if (c == 0)
        {
            return b;
        }
        a = b;
        b = c;
    }
    return 0;
}

int main(void)
{
    /* Insert DDR and PORT initializations */
    DDRA = 0x00;
    PORTA = 0xFF;
    DDRC = 0xFF;
    PORTC = 0x00;
    DDRD = 0xFF;
    PORTD = 0x00;
    /* Insert your solution below */

    static task task1, task2, task3;
    task *tasks[] = {&task1, &task2, &task3};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &open_tick;

    task2.state = start;
    task2.period = 300;
    task2.elapsedTime = task2.period;
    task2.TickFct = &lvl1_tick;

    //task3.state = start;
    //task3.period = 300;
    //task3.elapsedTime = task3.period;
    //task3.TickFct = &run_tick;

    task3.state = start;
    task3.period = 1;
    task3.elapsedTime = task3.period;
    task3.TickFct = &display_tick;

    unsigned short i;

    unsigned long GCD = tasks[0]->period;
    for (i = 1; i < numTasks; i++)
    {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    TimerSet(GCD);
    TimerOn();

    while (1)
    {
        for (i = 0; i < numTasks; i++)
        {
            if (tasks[i]->elapsedTime == tasks[i]->period)
            {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }
        while (!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}

