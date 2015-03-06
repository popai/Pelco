/*
 * usart.h
 *
 *  Created on: Jul 26, 2013
 *      Author: popai
 */

#ifndef USART_H
#define USART_H
/****************************************************************************************************/
#include <inttypes.h>
//Macros
#define RECEIVE_BUFF_SIZE 128
#define USART_RXC_VECT USART_RX_vect	//This change from mcu to mcu. This is for mega8, mega168, mega32
//#define F_CPU 12000000UL

#define UBRR_VALUE(baud) ((F_CPU/(baud<<4))-1)

//Varriables
volatile unsigned char URBuff[RECEIVE_BUFF_SIZE]; //USART Receive Buffer
//volatile int8_t UQFront;
//volatile int8_t UQEnd;

//Functions
void USARTInit(uint16_t ubrrvalue);

uint8_t UDataAvailable();

unsigned char UReadData();
void UWriteData(unsigned char data);
void UWriteInt(int);
void UWriteString(const char *str);

void UReadBuffer(void *buff, uint16_t len);
void UFlushBuffer();
int SerialRead(char *citit, uint16_t len);

/****************************************************************************************************/

#endif

