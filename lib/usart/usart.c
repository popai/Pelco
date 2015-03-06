/*
 * usart.c
 *
 *  Created on: Jul 26, 2013
 *      Author: popai
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
volatile uint8_t read_count, write_count;

void USARTInit(uint16_t ubrrvalue)
{
	read_count = 0;
	write_count = 0;

	//Set Baud rate
	//UBRRH=(unsigned char)(ubrrvalue>>8);
	//UBRRL=(unsigned char)ubrrvalue;

	UBRR0H = (unsigned char) (ubrrvalue >> 8); //Setting baudrate
	UBRR0L = (unsigned char) ubrrvalue; //Setting baudrate

	/*Set Frame Format

	 Asynchronous mode
	 No Parity
	 1 StopBit
	 char size 8

	 */

	//UCSRC=(1<<URSEL)|(3<<UCSZ0);(1 << USBS0) |
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //8N1...see Datasheet for more information

	/*Enable Interrupts
	 RXCIE- Receive complete
	 UDRIE- Data register empty

	 Enable The recevier and transmitter

	 */

	//UCSRB=(1<<RXCIE)|(1<<RXEN)|(1<<TXEN);( 1 << RXCIE0 )|
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); //Enable receiver and transmitter
	sei();

}

//The USART ISR
ISR(USART_RXC_VECT)
{
	//Read the data
	//unsigned char data = UDR0;
	//read_count = 0;
	if (write_count > RECEIVE_BUFF_SIZE - 1)
	{
		//UCSR0B &=~(1<<RXEN0);
		cli();
		//buff_count = 0;
	}
	//if (UDR0 == 0x1E)
		//write_count = 0; //start ader frame
	URBuff[write_count] = UDR0;
	++write_count;

	//leave interrupt service routine
	//reti();
}

unsigned char UReadData()
{
	unsigned char data;
	//Check if q is empty
	if (read_count == RECEIVE_BUFF_SIZE - 1)
	{
		//UCSR0B &= (1<<RXEN0);
		sei();
		data = URBuff[read_count];
		write_count = 0;
		read_count = 0;
		return data;
	}

	if (read_count == write_count)
	{
		//data = URBuff[read_count];
		write_count = 0;
		read_count = 0;
		return 0;
	}

	data = URBuff[read_count];
	++read_count;
	return data;
}

void UWriteData(unsigned char data)
{
	//Wait For Transmitter to become ready
	//while (!(UCSRA & (1 << UDRE)));
	while (!(UCSR0A & (1 << UDRE0)))
		;

	//Now write
	UDR0 = data;
}

uint8_t UDataAvailable()
{
	if (write_count - read_count > 0)
		return 1;
	else
		return 0;
}

void UWriteString(const char *str)
{
	while ((*str) != '\0')
	{
		UWriteData(*str);
		str++;
	}
}

void UWriteInt(int i)
{

	//! Local variables
	int ii;
	unsigned char int_buf[5];

	if (i < 0) //Integer is negative
	{
		i = -i; //Convert to positive Integer
		UWriteData('-'); //Print - sign
	}

	for (ii = 0; ii < 5;) //Convert Integer to char array
	{
		int_buf[ii++] = '0' + i % 10; //Find carry using modulo operation
		i = i / 10; //Move towards MSB
	}
	do
	{
		ii--;
	} while ((int_buf[ii] == '0') && (ii != 0)); //Remove leading 0's
	do
	{
		UWriteData(int_buf[ii--]);
	} while (ii >= 0); //Print int->char array convertion

}

void UReadBuffer(void *buff, uint16_t len)
{
	uint16_t i;
	for (i = 0; i < len; i++)
	{
		((char*) buff)[i] = UReadData();
	}
}
void UFlushBuffer()
{
/*
	while (UDataAvailable())
	{
		UReadData();
	}
*/
	write_count = read_count = 0;
	sei();

}

/***********************************************************
 *					 SERIAL READ
 Function read data on serial line
 - @citit: char array to hold data
 **********************************************************/
int SerialRead(char *citit, uint16_t len)
{
	int i = 0;
	while (UDataAvailable() && i < len)
	{
		citit[i++] = UReadData();
		//++i;
	}
	if (i > 0)
		citit[i - 1] = '\0';
	return i;
}
