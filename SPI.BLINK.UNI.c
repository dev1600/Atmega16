/*
 * SPI.BLINK.UNI.c
 *
 * Created: 08-02-2020 19:03:44
 * Author : apoor
 */ 

//MASTER
#include <avr/io.h>
#include <util/delay.h>
void SPI_MasterInit()
{
	DDRB |= (1<<PINB5) |(1<<PINB7) |(1<<PINB4);     //MOSI,SCK,SS is output
	DDRB &=~(1<<PINB6);     //MISO pin input
	SPCR |= (1<<SPE) |(1<<MSTR) |(1<<SPR0);     //SPI Enabled, SPI mode: master, Freq: /16
	//SPSR &=~(1<<SPI2X);     //Disable double speed
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
    while (1) 
    {	
		_delay_ms(5);
		if(PINB & (1<<PINB2))
		{
			SPI_Write(0b00000111);
			PORTB |=(1<<PINB3);
		}
		else
		{
			SPI_Write(0b11100000);
			PORTB &=~(1<<PINB3);
		}
    }
}
/*
*/








//SLAVE
#include <avr/io.h>
#include <util/delay.h>
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
int main(void)
{
	SPI_SlaveInit();
	DDRB |=(1<<PINB0);
	while(1)
	{
		if(SPI_Receive()==0b00000111)
			PORTB &=~(1<<PINB0);
		else
			PORTB |=(1<<PINB0);
	}
}
/*
*/