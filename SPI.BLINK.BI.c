/*
 * SPI.BLINK.BI.c
 *
 * Created: 19-02-2020 19:53:54
 * Author : apoorv
 */ 

//MASTER
#include <avr/io.h>
#include <util/delay.h>
void blink(int k)
{
	while(k!=0)
	{
		PORTB |=(1<<PINB3);
		_delay_ms(200);
		PORTB &=~(1<<PINB3);
		_delay_ms(200);
		k = k-1;
	}
}
void SPI_MasterInit()
{
	DDRB |= (1<<PINB5) |(1<<PINB7) |(1<<PINB4);     //MOSI,SCK,SS is output
	DDRB &=~(1<<PINB6);     //MISO pin input
	SPCR |= (1<<SPE) |(1<<MSTR) |(1<<SPR0);     //SPI Enabled, SPI mode: master, Freq: /16
}
char SPI_Receive(void)
{
	while(!(SPSR & (1<<SPIF)));     //Wait till reception is complete
	return(SPDR);     //Return received data
}
void SPI_Write(char data)
{
	SPDR = data;     //Write data to SPI register
	while(!(SPSR & (1<<SPIF)));     //Wait till transmission complete
}
int main(void)
{
	DDRB &=~(1<<PINB2);     //Declare switch as input
	PORTB |=(1<<PINB2);
	DDRB |=(1<<PINB3);     //Declare LED as output
	SPI_MasterInit();
	int flag = 0;
	int status = 0;
	while (1)
	{
		_delay_ms(5);
		status = PINB&(1<<2);
		if(status)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b11100000);
			_delay_us(100);
		}
		else
		{
			SPI_Write(0b00000000);
			_delay_us(100);
			flag = SPI_Receive();
			_delay_us(100);
			if(flag == 0b11100000)
				blink(3);
			else if(flag == 0b00000000)
				PORTB &=~(1<<PINB3);
		}
	}
}
/*
*/










//SLAVE
#include <avr/io.h>
#include <util/delay.h>
void blink(int k)
{
	while(k!=0)
	{
		PORTB |=(1<<PINB0);
		_delay_ms(200);
		PORTB &=~(1<<PINB0);
		_delay_ms(200);
		k = k-1;
	}
}
void SPI_SlaveInit()
{
	DDRB &= ~((1<<PINB5) |(1<<PINB7) |(1<<PINB4));     //MOSI,SCK,SS defined as input
	DDRB |=(1<<PINB6);     //Make MISO pin as output
	SPCR |=(1<<SPE) |(1<<SPR0);     //Enable SPI slave mode, Freq: /16
}
char SPI_Receive(void)
{
	while(!(SPSR & (1<<SPIF)));     //Wait till reception is complete
	return(SPDR);     //Return received data
}
void SPI_Write(char data)
{
	SPDR = data;     //Write data to SPI register
	while(!(SPSR & (1<<SPIF)));     //Wait till transmission complete
}
int main(void)
{
	DDRB &=~(1<<PINB2);     //Declare switch as input
	PORTB |=(1<<PINB2);
	DDRB |=1<<PINB0;     //Declare LED as output
	SPI_SlaveInit();
	int flag = 0;
	int status = 0;
	while(1)
	{
		_delay_ms(5);
		status = PINB&(1<<2);
		if(status)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b11100000);
			_delay_us(100);
		}
		else
		{
			SPI_Write(0b00000000);
			_delay_us(100);
			flag = SPI_Receive();
			_delay_us(100);
			if( flag == 0b11100000)
				blink(3);
			else if(flag == 0b00000000)
				PORTB &=~(1<<PINB0);
		}
	}
}
/*
*/