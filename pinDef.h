/*
 * pinDef.h
 *
 *  Created on: Apr 14, 2013
 *      Author: popai
 */

#ifndef PINDEF_H_
#define PINDEF_H_

#include <avr/io.h>

//definiti relee
#define sus 	PD2   // ' Tilt up
#define jos 	PD3   // ' Tilt down
#define st 		PD4   // ' Pan Left
#define dr 		PD5   // ' Pan Right
#define z_out 	PD6   //' Zum out
#define z_in 	PD7   // ' Zum in
#define f_out 	PB0   //' focus out
#define f_in 	PB1   // ' focus in

//definiti pentru adresa
#define LED    PB2   // led
#define a0     PC3   //pin 1 = digit 0
#define a1     PC4   //pin 2 = digit 1
#define a2     PC5   //pin 3 = digit 2


void pinSetUp();


#endif /* PINDEF_H_ */
