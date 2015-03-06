/*
 * pelco.c
 *
 *  Created on: Feb 16, 2015
 *      Author: popai
 *
 * @breaf: Digital ignition for 4 stroke turbo engines
 * NOTE: use compiler: winavr
 * Digital parts: Atmega328, XTAL 16MHz
 */

#include <stdio.h>
#include <util/delay.h>

#include "pindef.h"
#include "lib/usart/usart.h"

#define output_low(x)	PORTD &= ~(1 << x)
#define output_high(x)	PORTD |= (1 << x)
#define outputB_low(x)	PORTB &= ~(1 << x)
#define outputB_high(x)	PORTB |= (1 << x)

void GetADD();
void GetPacket();
void HandlePacket();

int8_t tmp1 = 0; // = b0 //temparary storage variable
int8_t stx = 0; // = b1 //Sync Byte = 255 (always = 255)
int8_t adr = 0; // = b2 //Address of Pelco device being pinged
int8_t cmd1 = 0; //= b3 //First Command (used to control relays)
int8_t cmd2 = 0; //= b4 //Second Command (used to control relays)
int8_t dat1 = 0; // = b5 //Data for eeprom (not used by this decoder)
int8_t dat2 = 0; //= b6 //Data for eeprom (not used by this decoder)
int8_t chks = 255; // = b7 //Checksum (adr+cmd1+cmd2+dat1+dat2 = chks)
int8_t my_adr = 255; // = b8 //Address of Decoder
//Symbol ml = 1 //EEPROM memory location
//Symbol RX_PIN = C.5 //Pin 4 (Receives RS-232 TTL from RS485 chip)
//Symbol RX_BAUD = T4800 //Baud rate 4800 baud @ 8N1
//Symbol pgm = pinC.1 //Pin 18 (Program RS-485 address into PICAXE)
//Symbol latch = pinC.0 //Pin 17 (If low, enables Latch command for relays)
//enabletime //enables internal clock
//disconnect //disconnects programming port to speed up processing

int main(void)
{
	pinSetUp();
	USARTInit(103);	//bud rate 9600
	//USARTInit(8); //bud rate 115200
	_delay_ms(20);
	UWriteString("Program pornit");
	UWriteData(0x0D);
	UWriteData(0x0A);

	//citim adresa decodor
	GetADD();

	while (1)
	{
		GetPacket();
		if (adr == my_adr)
			HandlePacket();
		_delay_ms(30);
	}

}

/**
 * @breaf Read pins address and set the address
 *
 * @parameter
 */
void GetADD() //
{
	int8_t i = 0;
	if ((PINC & (1 << PC3)) == 0)
		i |= (1 << 0);
	if ((PINC & (1 << PC4)) == 0)
		i |= (1 << 1);
	if ((PINC & (1 << PC5)) == 0)
		i |= (1 << 2);

	char buffer[] = "########";
	sprintf(buffer, "adr:%d %c%c", i, 0x0D, 0x0A);
	UWriteString(buffer);
	my_adr = i;
	for (tmp1 = 1; tmp1 <= my_adr; tmp1++)
	{
		PORTB |= (1 << LED);
		_delay_ms(200);
		PORTB &= ~(1 << LED);
		_delay_ms(200);
	}
}

/**
 * @breaf citeste freimul dat de dvr
 *'Listen for command on RS485 bus
 * @parameter
 */
void GetPacket()
{
	do
	{
		stx = UReadData();
		if (stx == 0xFF)
		{
			adr = UReadData();
			cmd1 = UReadData();
			cmd2 = UReadData();
			dat1 = UReadData();
			dat2 = UReadData();
			chks = UReadData();
			tmp1 = adr + cmd1 + cmd2 + dat1 + dat2;
		}
		UWriteData(tmp1);
	} while (tmp1 != chks);
}

/**
 * @breaf Executa comenzile citite din frame
 * 'Code turns off opposite direction relay to prevent damage
 *'before sending new command to turn on or toggle relay.
 * @parameter
 */
void HandlePacket()
{

	if (cmd2 == 2)
	{
		output_low( st);
		output_high( dr);
	} //Pan Right
	if (cmd2 == 4)
	{
		output_low( dr);
		output_high( st);
	} //Pan Left
	if (cmd2 == 8)
	{
		output_low( jos);
		output_high( sus);
	} //Tilt Up
	if (cmd2 == 10)
	{
		output_low( sus);
		output_high( jos);
	} //Tilt Down
	if (cmd2 == 20)
	{
		output_low( z_out);
		output_high( z_in);
	} //Zoom in
	if (cmd2 == 40)
	{
		output_low( z_in);
		output_high( z_out);
	} //Zoom out
	if (cmd1 == 1)
	{
		output_low( f_out);
		output_low(f_in);
	}
	if (cmd2 == 80)
	{
		outputB_low( f_in);
		outputB_low( f_out);
	}
	//delay_ms(500);
	if (cmd1 == 0 && cmd2 == 0)
	{
		output_low(sus);
		output_low(jos);
		output_low(st);
		output_low(dr);
		output_low(z_in);
		output_low(z_out);
		outputB_low(f_in);
		outputB_low(f_out);
	}
}
