/*	Author: dshan017
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1EfkRixsrk7c_f4LpJ3mL2n_2pM59C4Rx/view?usp=drivesdk
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
unsigned char prev = 0;
unsigned char win = 0;
unsigned char lose = 0;
unsigned char runrow = 0x1F;
unsigned char runpattern = 0x00;
unsigned char moverow = 0x1F;
unsigned char movepattern = 0x00;

char level1[] = {0, 0, 1, 0, 0, 0, 0, 0};
char level2[] = {0, 0, 0, 0, 1, 0, 0, 0};
char level3[] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
char level4[] = {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0};
char level5[] = {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0};
char level6[] = {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
char level7[] = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0};

enum run_states
{
    run_wait,
    run_start,
    run_start2,
    run_start3,
    run_start4,
    run_start5,
    run_start6,
    run_start7,
    run_setup,
    run_setup2,
    run_setup3,
    run_setup4,
    run_setup5,
    run_setup6,
    run_setup7,
    run_fail,
    run_win
};

int run_tick(int state)
{

    unsigned char begin = ~PINA & 0x20;
    unsigned char jump = ~PINA & 0x01;
    unsigned char jump2 = ~PINA & 0x02;
    unsigned char next = ~PINA & 0x04;
    unsigned char multip = ~PINA & 0x80;

    switch (state)
    {
    case run_wait:
	win = 0;
	lose = 0;
        if ((begin && (prev == 0)) || multip) {
            state = run_setup;
        }
	    else if (begin && (prev == 1)) {
	        state = run_setup2;
	    }
        else if (begin && (prev == 2)) {
            state = run_setup3;
        }
        else if (begin && (prev == 3)) {
            state = run_setup4;
        }
        else if (begin && (prev == 4)) {
            state = run_setup5;
        }
        else if (begin && (prev == 5)) {
            state = run_setup6;
        }
        else if (begin && (prev == 6)) {
            state = run_setup7;
        }
        else {
            state = run_wait;
        }
        break;

    case run_setup:
        state = run_start;
        break;

    case run_setup2:
	    state = run_start2;
	    break;

    case run_setup3:
        state = run_start3;
        break;

    case run_setup4:
        state = run_start4;
        break;

    case run_setup5:
        state = run_start5;
        break;

    case run_setup6:
        state = run_start6;
        break;

    case run_setup7:
        state = run_start7;
        break;

    case run_start:
        if (i < 6) {
            if (level1[i] == 1)
            {
                if (jump || jump2)
                {
                    i++;
                    state = run_start;
                }
                else
                {
                    i = 0;
                    state = run_fail;
                }
            }
            else
            {
		        i++;
                state = run_start;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_start2:
        if (i < 6) {
            if (level2[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start2;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start2;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_start3:
        if (i < 12) {
            if (level3[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start3;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start3;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_start4:
        if (i < 12) {
            if (level4[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start4;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start4;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_start5:
        if (i < 18) {
            if (level5[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start5;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start5;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_start6:
        if (i < 18) {
            if (level6[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start6;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start6;
            }
        }
        else {
            state = run_win;
        }
        break;
        
    case run_start7:
        if (i < 24) {
            if (level7[i] == 1) {
                if (jump || jump2) {
                    i++;
                    state = run_start7;
                }
                else {
                    i = 0;
                    state = run_fail;
                }
            }
            else {
                i++;
                state = run_start7;
            }
        }
        else {
            state = run_win;
        }
        break;

    case run_fail:
	    lose = 1;
        if (begin && (prev == 0))
        {
	        lose = 0;
            state = run_setup;
        }
	    else if (begin && (prev == 1)) {
		    lose = 0;
		    state = run_setup2;
	    }  
        else if (begin && (prev == 2)) {
		    lose = 0;
		    state = run_setup3;
	    } 
        else if (begin && (prev == 3)) {
		    lose = 0;
		    state = run_setup4;
	    }   
        else if (begin && (prev == 4)) {
		    lose = 0;
		    state = run_setup5;
	    }  
        else if (begin && (prev == 5)) {
		    lose = 0;
		    state = run_setup6;
	    }  
        else if (begin && (prev == 6)) {
		    lose = 0;
		    state = run_setup7;
	    }  
        else
        {
            state = run_fail;
        }
        break;

    case run_win:
	win = 1;
	if (next) {
		prev++;
		win = 0;
		i = 0;
		state = run_wait;
	}
	else {
		state = run_win;
	}
        break;

    default:
        state = run_wait;
        break;
    }

    switch (state) {
        case run_wait:
            runrow = 0x1F;
            runpattern = 0x00;
            moverow = 0x1F;
            movepattern = 0x00;
            break;

        case run_setup:
            runrow = 0x0F;
            runpattern = 0x08;
	        movepattern = 0x80;
            break;

	    case run_setup2:
	        runrow = 0x0F;
	        runpattern = 0x02;
	        movepattern = 0x80;
	        break;

        case run_setup3:
            if (i < 6) {
                runrow = 0x0F;
	            runpattern = 0x04;
	            movepattern = 0x80;
            }
            else {
                runrow = 0x0F;
	            runpattern = 0x10;
	            movepattern = 0x80; 
            }
	        break;

        case run_setup4:
            if (i < 6) {
                runrow = 0x0F;
	            runpattern = 0x02;
	            movepattern = 0x80;
            }
            else {
                runrow = 0x0F;
	            runpattern = 0x20;
	            movepattern = 0x80;
            }
            break;

        case run_setup5:
            if (i < 6) {
                runrow = 0x0F;
	            runpattern = 0x08;
	            movepattern = 0x80;
            }
            else if (i < 12) {
                runrow = 0x0F;
	            runpattern = 0x20;
	            movepattern = 0x80;
            }
            else {
                runrow = 0x0F;
                runpattern = 0x04;
                movepattern = 0x80;
            }
            break;

        case run_setup6:
            if (i < 6) {
                runrow = 0x0F;
	            runpattern = 0x10;
	            movepattern = 0x80;
            }
            else if (i < 12) {
                runrow = 0x0F;
	            runpattern = 0x08;
	            movepattern = 0x80;
            }
            else {
                runrow = 0x0F;
                runpattern = 0x40;
                movepattern = 0x80;
            }
            break;

        case run_setup7:
            if (i < 6) {
                runrow = 0x0F;
	            runpattern = 0x08;
	            movepattern = 0x80;
            }
            else if (i < 12) {
                runrow = 0x0F;
	            runpattern = 0x10;
	            movepattern = 0x80;
            }
            else if (i < 18) {
                runrow = 0x0F;
                runpattern = 0x20;
                movepattern = 0x80;
            }
            else {
                runrow = 0x0F;
                runpattern = 0x02;
                movepattern = 0x80;
            }
            break;

        case run_start:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
		        moverow = 0x01;
            }
	        else if (movepattern == 0x01)
            {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                else {
                    moverow = 0x07;
                }
                movepattern >>= 1;
            }
            break;

        case run_start2:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;

        case run_start3:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;
        
        case run_start4:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;

        case run_start5:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;
        
        case run_start6:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;

        case run_start7:
            moverow = 0x07;
            if (movepattern == 0x01 && (jump || jump2)) {
                movepattern = 0x80;
                moverow = 0x01;
            }
            else if (movepattern == 0x01) {
                movepattern = 0x80;
            }
            else {
                if (jump || jump2) {
                    moverow = 0x01;
                }
                movepattern >>= 1;
            }
            break;

	    case run_fail:
	    	movepattern = 0x00;
            moverow = 0x1F;
            runpattern = 0x00;
            runrow = 0x1F;
		    break;

	    case run_win:
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
	    if (win) {
		state = win_s2;
	    }
	    else {
            	state = win_s2;
	    }
	    break;

        case win_s2:
	    if (win) {
		state = win_s3;	
	    }
	    else {
		state = win_wait;
	    }
            break;

        case win_s3:
	    if (win) {
		state = win_s4;
	    }
	    else {
		state = win_wait;
	    }
            break;

        case win_s4:
	    if (win) {
            	state = win_s5;
	    }
	    else {
		state = win_wait;
	    }
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
    task2.period = 300;
    task2.elapsedTime = task2.period;
    task2.TickFct = &run_tick;

    task3.state = start;
    task3.period = 1;
    task3.elapsedTime = task3.period;
    task3.TickFct = &win_tick;

    task4.state = start;
    task4.period = 1;
    task4.elapsedTime = task4.period;
    task4.TickFct = &lose_tick;

    task5.state = start;
    task5.period = 1;
    task5.elapsedTime = task5.period;
    task5.TickFct = &display_tick;

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
