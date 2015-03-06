/*
 * pinDef.c
 *
 *  Created on: Apr 14, 2013
 *      Author: popai
 */
#include <avr/io.h>

void pinSetUp()
{
	DDRD =  0b11111110;
	PORTD = 0b00000001;

	DDRB =  0b11111111;
	PORTB = 0b00000000;

	DDRC =  0b00000000;
	PORTC = 0b00111000;
}
