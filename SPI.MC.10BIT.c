/*
 * SPI.MC.10BIT.c
 *
 * Created: 23-02-2020 13:46:08
 * Author : apoorv
 */ 
/*
//MASTER : TRANSMIT
#define F_CPU 1000000UL
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
int adc_read( unsigned char dig)
{
	dig = dig & 0b00000111;
	ADMUX |= dig; //declaring pin
	ADCSRA |=(1<<ADSC); //start conversion
	while(!(ADCSRA & (1<<ADIF))); //wait for conversion;
	ADCSRA |=(1<<ADIF); // rewrite to set 1
	uint8_t low = ADCL;
	uint16_t high = ADCH<<8 | low;
	return (high);
}
int main(void)
{
	SPI_MasterInit();
	DDRA = 0b00000000;
	int x=0,y=0,x2=0,y2=0;
	int p1,p2;
	ADMUX = 0b01000000;
	//int delay = 0;
	ADMUX |=(1<<REFS0);
	ADMUX &=~(1<<ADLAR);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //set pre scaler: /16
	while (1)
	{
		
		switch(ADMUX)
		{
			case 0b01000000:
				x = adc_read(0);
				p1 = (x & 0b00001111);
				x = (x>>4);
				p2 = (x & 0b00111111);
				x2 = (p1 | 0b00000001);
				//_delay_us(delay);
				SPI_Write(x2);
				//_delay_us(delay);
				SPI_Write(p2);
				ADMUX = 0b01000001;
				break;
			case 0b01000001:
				y = adc_read(1);
				p1 = (y & 0b00001111);
				y = (y>>4);
				p2 = (y & 0b00111111);
				y2 = (p1 & 0b11111110);
				//_delay_us(delay);
				SPI_Write(y2);
				//_delay_us(delay);
				SPI_Write(p2);
				ADMUX = 0b01000000;
				break;
			default:
				break;
		}
		ADCSRA |=(1<<ADSC);
	}

}
/*
*/









//RECEIVE
//Let OCR0 be BL
//Let OCR2 be BR
//Let PB0 be FL
//Let PB1 be FR

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
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
void pwm_set()
{
	TCCR0 |= (1<<WGM00) |(1<<WGM01); //fast pwm
	TCCR0 |= (1<<COM01) |(1<<CS00);  //OC0 and prescaler set
	TCCR2 |= (1<<WGM20) |(1<<WGM21); //fast pwm
	TCCR2 |= (1<<COM21) |(1<<CS20);  //Oc2 and prescaler set
	DDRB |= 1<<PINB3;
	DDRD |= 1<<PIND7;
}
int mapp(float a, float in_min, float in_max, float out_min, float out_max)
{
	return (a - in_min) * (out_max - out_min)/(in_max - in_min) + out_min;
}
int x=0,y=0;
int xval=0,yval=0;
int main(void)
{
	DDRB |=(1<<PINB1) |(1<<PINB0);
	SPI_SlaveInit();
	pwm_set();
	uint16_t val=0;
	int low,high;
	int buffer = 35;
	int delay = 280;
	while(1)
	{
		_delay_us(delay);
		high = SPI_Receive();
		_delay_us(delay);
		low = SPI_Receive();
		_delay_us(delay);
		val = (low<<4 | high);
		if((val & 0b00000001) == 1)
		{
			x = mapp(val,0,1023,-255,255);
		}
		else if((val & 0b00000001) == 0)
		{
			y = mapp(val,0,1023,-255,255);
		}
		
		if(x>230)
			x = 255;
		else if(x<-230)
			x = -255;
		
		if(y>230)
			y = 255;
		else if(y<-230)
			y = -255;
		
		if(abs(x)< 80 && abs(y)<80) // no motion
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			OCR0 = 0;
			OCR2 = 0;
		}
		
		else if(abs(x) < 10 && y < 0) //forward
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			OCR0 = abs(y);
			OCR2 = abs(y);
		}
		
		else if(abs(x) < 20 && y > 0) //backward
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			OCR0 = abs(y);
			OCR2 = abs(y);
		}
		
		else if(abs(y)<40 && x > 0) //spot turn right
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			OCR0 = abs(x);
			OCR2 = abs(x);
		}
		
		else if(abs(y)<20 && x < 0) //spot turn left
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			OCR0 = abs(x);
			OCR2 = abs(x);
		}
		
		else if(x>0 && y <0 && x>=abs(y))	//octet 1
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval<buffer) yval = 0;
			OCR0 = x;
			OCR2 = yval;
		}
		
		else if(x>0 && y<0 && abs(y)>x)	//octet 2
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			xval = abs(255-x);
			if(xval<buffer) xval = 0;
			OCR0 = abs(y);
			OCR2 = xval;
		}
		
		else if(x<0 && y<0 && abs(y)>abs(x))	//octet 3
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<buffer)	xval = 0;
			OCR0 = xval;
			OCR2 = abs(y);
		}
		
		else if(x<0 && y<0 && abs(x)>=abs(y))	//octet 4
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval <buffer) yval = 0;
			OCR0 = yval;
			OCR2 = abs(x);
		}
		
		else if(x<0 && y>0 && abs(x)>y)	//octet 5
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval<buffer)	yval = 0;
			OCR0 = abs(x);
			OCR2 = yval;
		}
		
		else if(x<0 && y>0 && y>=abs(x)) //octet 6
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<buffer)	xval = 0;
			OCR0 = y;
			OCR2 = xval;
		}
		
		else if(x>0 && y>0 && y>=abs(x))	//octet 7
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<buffer)	xval = 0;
			OCR0 = xval;
			OCR2 = y;
		}
		
		else if(x>0 && y>0 && x>y)	//octet 8
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval<buffer)	yval = 0;
			OCR0 = yval;
			OCR2 = x;
		}
	}
}
/*
*/