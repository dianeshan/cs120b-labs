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
//unsigned char prev = 0;
unsigned char win = 0;
unsigned char lose = 0;
unsigned char runrow = 0x1F;
unsigned char runpattern = 0x00;
unsigned char moverow = 0x1F;
unsigned char movepattern = 0x00;

unsigned char lvl1pattern[] = {0x10, 0x04};
unsigned char lvl1row[] = {0x0F, 0x1E};
unsigned char lvl1[] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0};
int period1 = 0;
int i = 0;

int lvl1_tick(int state) {
    unsigned char begin = ~PINA & 0x20;
    unsigned char jump = ~PINA & 0x01;
    unsigned char duck = ~PINA & 0x02;
    unsigned char jump2 = ~PINA & 0x04;
    unsigned char duck2 = ~PINA & 0x08; 
    unsigned char next = ~PINA & 0x04;

    switch (state) {
        case lvl1_wait:
            if (begin) {
                state = lvl1_run;
            }
            else {
                state = lvl1_wait;
            }
            break;

        case lvl1_run:
            period1 = period1 + 2;
            if (i < 16) {
                if (lvl1[i] == 1) {
                    if (jump || jump2) {
                        i++;
                        state = lvl1_pause;
                    }
                    else {
                        i = 0;
                        state = lvl1_fail;
                    }
                }
                else if (lvl1[i] == 2) {
                    if (duck || duck2) {
                        i++;
                        state = lvl1_pause;
                    }
                    else {
                        i = 0;
                        state = lvl1_fail;
                    }
                }
                else {
                    i++;
                    state = lvl1_pause;
                }
            }
            else {
                i = 0;
                state = lvl1_win;
            }
            break;
        
        case lvl1_pause:
            period1 = period1 + 2;
            if (period1 == 300) {
                period1 = 0;
                state = lvl1_run;
            }
            else {
                state = lvl1_pause;
            }
            break;

        case lvl1_fail:
            lose = 1;
            if (begin) {
                lose = 0;
                state = lvl1_run;
            }
            else {
                state = lvl1_fail;
            }
            break;

        case lvl1_win:
            win = 1;
            if (next) {
                win = 0;
                state = lvl1_wait;
            }
            else {
                state = lvl1_win;
            }
            break;

        default:
            state = lvl1_wait;
            break;
    }

    switch (state) {
        case lvl1_wait:
            runrow = 0x1F;
	        runpattern = 0x00;
	        moverow = 0x1F;
	        movepattern = 0x00;
            break;

        case lvl1_run: //run every 2 ms
            if (i < 8) {
                runrow = 0x0F;
                runpattern = 0x10; 
            }
            else {
                runrow = 0x1E;
                runpattern = 0x04;
            }
            break;

	    case lvl1_pause:
	    	break;

	    case lvl1_fail:
	    	movepattern = 0x00;
		moverow = 0x1F;
		runpattern = 0x00;
		runrow = 0x1F;
		break;

        case lvl1_win:
            movepattern = 0x00;
		    moverow = 0x1F;
		    runpattern = 0x00;
		    runrow = 0x1F;
		    break;

        default:
            break;
    }

    return state;
}

//how the person runs
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

enum lose_states { lose_wait, lose_s1, lose_s2, lose_s3 };
unsigned char loserow = 0x1F;
unsigned char losepattern = 0x00;

int lose_tick(int state) {
    switch (state) {
        case lose_wait:
            if (lose) {
                state = lose_s1;
            }
            else {
                state = lose_wait;
            }
            break;

        case lose_s1:
            state = lose_s2;
            break;

        case lose_s2:
            state = lose_s3;
            break;

        case lose_s3:
            if (lose) {
                state = lose_s1;
            }
            else {
                state = lose_wait;
            }
            break;

        default:
            state = lose_wait;
            break;
    }

    switch (state) {
        case lose_wait:
            loserow = 0x1F;
            losepattern = 0x00;
            break;

        case lose_s1:
            loserow = 0x00;
            losepattern = 0x20;
            break;

        case lose_s2:
            loserow = 0x0F;
            losepattern = 0x10;
            break;

        case lose_s3:
            loserow = 0x0F;
            losepattern = 0x08;
            break;

        default:
            break;
    }

    return state;
}

enum win_states { win_wait, win_s1, win_s2, win_s3, win_s4, win_s5 };
unsigned char winrow = 0x1F;
unsigned char winpattern = 0x00;

int win_tick(int state) {
    switch(state) {
        case win_wait:
            if (win) {
                state = win_s1;
            }
            else {
                state = win_wait;
            }
            break;

        case win_s1:
            state = win_s2;
            break;

        case win_s2:
            state = win_s3;
            break;

        case win_s3:
            state = win_s4;
            break;

        case win_s4:
            state = win_s5;
            break;

        case win_s5:
            if (win) {
                state = win_s1;
            }
            else {
                state = win_wait;
            }
            break;

        default:
            state = win_wait;
            break;
    }

    switch (state) {
        case win_wait:
            winrow = 0x1F;
            winpattern = 0x00;
            break;

        case win_s1:
            winrow = 0x00;
            winpattern = 0x40;
            break;

        case win_s2:
            winrow = 0x17;
            winpattern = 0x20;
            break;

        case win_s3:
            winrow = 0x1B;
            winpattern = 0x10;
            break;

        case win_s4:
            winrow = 0x17;
            winpattern = 0x08;
            break;

        case win_s5:
            winrow = 0x00;
            winpattern = 0x04;
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
        finalpattern = runpattern | openpattern | movepattern | winpattern | losepattern;
        finalrow = runrow & openrow & moverow & winrow & loserow;
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

    static task task1, task2, task3, task4, task5;
    task *tasks[] = {&task1, &task2, &task3, &task4, &task5};
    const unsigned short numTasks = sizeof(tasks) / sizeof(task *);

    const char start = -1;

    task1.state = start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &open_tick;

    task2.state = start;
<<<<<<< HEAD
    task2.period = 2;
=======
    task2.period = 300;
>>>>>>> c4347407dcbdd6a5115325d723a24baa11790d6a
    task2.elapsedTime = task2.period;
    task2.TickFct = &lvl1_tick;
 
    task3.state = start;
<<<<<<< HEAD
    task3.period = 300;
    task3.elapsedTime = task3.period;
    task3.TickFct = &move_tick;
=======
    task3.period = 1;
    task3.elapsedTime = task3.period;
    task3.TickFct = &win_tick;
>>>>>>> c4347407dcbdd6a5115325d723a24baa11790d6a

    task4.state = start;
    task4.period = 1;
    task4.elapsedTime = task4.period;
    task4.TickFct = &lose_tick;

    task5.state = start;
    task5.period = 1;
    task5.elapsedTime = task5.period;
    task5.TickFct = &display_tick;
/*
    task6.state = start;
    task6.period = 1;
    task6.elapsedTime = task6.period;
    task6.TickFct = &display_tick;
*/
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
