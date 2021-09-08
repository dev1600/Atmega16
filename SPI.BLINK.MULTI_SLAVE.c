/*
 * SPI.BLINK.MULTI_SLAVE.c
 *
 * Created: 05-03-2020 16:40:13
 * Author : Apoorv
 */ 
/*
//MASTER
#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL
void blink(int k)
{
	while(k!=0)
	{
		PORTB |=(1<<PINB0);
		_delay_ms(100);
		PORTB &=~(1<<PINB0);
		_delay_ms(100);
		k = k-1;
	}
}
void SPI_MasterInit()
{
	DDRB |= (1<<PINB5) |(1<<PINB7) |(1<<PINB4);     //MOSI,SCK,SS is output
	DDRB &=~(1<<PINB6);     //MISO pin input
	SPCR |= (1<<SPE) |(1<<MSTR) |(1<<SPR0);     //SPI Enabled, SPI mode: master, Freq: /16
}
void SPI_Receive(void)
{
	while(!(SPSR & (1<<SPIF)));     //Wait till reception is complete
	//return(SPDR);     //Return received data
	if(SPDR == 0b000000111)
		blink(3);
}
void SPI_Write(char data)
{
	SPDR = data;     //Write data to SPI register
	while(!(SPSR & (1<<SPIF)));     //Wait till transmission complete
}
int main(void)
{
	DDRB &=~(1<<PINB2);     //Declare switch as Input for Slave 1
	PORTB |=(1<<PINB2);
	DDRD &=~(1<<PIND2);     //Declare switch as Input for Slave 2
	PORTD |=(1<<PIND2);
	DDRB |=(1<<PINB0);     //Declare LED as output
	SPI_MasterInit();
    while (1) 
    {
		//Transmit:
		_delay_ms(5);
		if(PINB & 1<<PINB2)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b10000111);     //Slave 1 Addr
			_delay_us(100);
		}
		_delay_us(5);
		if(PIND & 1<<PIND2)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b01000111);     //Slave 2 Addr
			_delay_us(100);
		}
		else
			SPI_Write(0b00000000);
			
		//Receive:
		SPI_Receive();		
    }
}
/*
*/




/*
//SLAVE 1
#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL
void blink(int k)
{
	while(k!=0)
	{
		PORTB |=(1<<PINB0);
		_delay_ms(100);
		PORTB &=~(1<<PINB0);
		_delay_ms(100);
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
	int flag = 0;
	SPI_SlaveInit();
	while(1)
	{
		//Transmit:
		_delay_ms(5);
		if(PINB & 1<<PINB2)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b00000111);
			_delay_us(100);
		}
		
		//Receive:
		flag = SPI_Receive();
		if(flag == 0b10000111)
			blink(3);
		else if(flag == 0b01000111)
		{
			_delay_us(100);
			SPI_Write(flag);
			_delay_us(100);
		}
	}
}
/*
*/




/*
//SLAVE 2
#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 1000000UL
void blink(int k)
{
	while(k!=0)
	{
		PORTB |=(1<<PINB0);
		_delay_ms(100);
		PORTB &=~(1<<PINB0);
		_delay_ms(100);
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
	int flag = 0;
	SPI_SlaveInit();
	while(1)
	{
		//Transmit:
		_delay_ms(5);
		if(PINB & 1<<PINB2)
		{
			blink(1);
			_delay_us(100);
			SPI_Write(0b00000111);
			_delay_us(100);
		}
		
		//Receive:
		flag = SPI_Receive();
		if(flag == 0b01000111)
			blink(3);
		else if(flag == 0b00000111)
		{
			_delay_us(100);
			SPI_Write(flag);
			_delay_us(100);
		}
	}
}
/*
*/