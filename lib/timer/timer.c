/*
 * timer.c
 *
 *  Created on: Dec 18, 2013
 *      Author: popai
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

volatile uint8_t seconds;
volatile uint8_t minutes;
volatile uint8_t houers;

/*
 * 			Timer1 setup
 *
 * F_CPU = 16MHz, prescaler = 1024
 * timer resolution = 1/(16*10^6 / 1024) = 	6.4e-5 s
 *(target time) = (timer resolution) * (# timer counts + 1)
 *(target (# timer counts + 1) = (target time) / (timer resolution)
 *(target (# timer counts + 1) = (1 s) / (6.4e-5 s)
 *(target (# timer counts + 1) = 15625
 *(target (# timer counts) = 15625 - 1 = 15624
 * Timer mode = CTC mode, when the timer matches our desired count it will reset itself to zero.
 */

void TimeInit()
{
    // initialize Timer1
    cli();          // disable global interrupts
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B

    // set compare match register to desired timer count:
    OCR1A = 15624;
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
    seconds = 0;
    minutes = 0;
    sei();          // enable global interrupts
}

/*
 * Interrupt service rutin for timer
 *
 * Since we’ve enabled an overflow interrupt,
 * it will call the ISR(TIMER1_COMPA_vect) whenever the timer overflows.
 */

ISR(TIMER1_COMPA_vect)
{
	++seconds;
	if(seconds == 60)
	{
		seconds = 0;
		++minutes;
	}

	if(minutes == 60)
	{
		minutes = 0;
		++houers;
	}
}

uint8_t GetSecunds()
{
	return seconds;
}

uint8_t GetMinutes()
{
	return minutes;
}
uint8_t GetHouers()
{
	return houers;
}

