/*
 * SPI.MC.c
 *
 * Created: 22-02-2020 18:55:50
 * Author : apoorv
 */ 

//MASTER : TRANSMIT 
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
	ADMUX = dig; //declaring pin
	ADMUX |=(1<<REFS0);
	ADMUX &=~(1<<ADLAR);
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
	int x,y;
	ADMUX = 0b01000000;
	ADCSRA = (1<<ADEN)|(1<<ADPS2); //set prescaler: /16
	while (1)
	{
		switch(ADMUX)
		{
			case 0b01000000:
				x = adc_read(0)/4;
				x = (x | 0b00000001);
				_delay_us(10);	
				SPI_Write(x);
				//_delay_us(10);
				ADMUX = 0b01000001;			
				break;
			case 0b01000001:
				y = adc_read(1)/4;
				y = (y & 0b11111110);
				_delay_us(10);	
				SPI_Write(y);
				//_delay_us(10);	
				ADMUX = 0b01000000;
				break;
		}
		ADCSRA |=1<<ADSC;
	}

}
/*
*/










//SLAVE : RECEIVE
//Let OCR0 be BL
//Let OCR2 be BR
//Let PB0 be FL
//Let PB1 be FR
/*
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
void pwm_set()
{
	TCCR0 |= (1<<WGM00) |(1<<WGM01); //fast pwm
	TCCR0 |= (1<<COM01) |(1<<CS00);  //OC0 and prescaler set
	TCCR2 |= (1<<WGM20) |(1<<WGM21); //fast pwm
	TCCR2 |= (1<<COM21) |(1<<CS20);  //Oc2 and prescaler set
	DDRB |= 1<<PINB3;
	DDRD |= 1<<PIND7;
}

char SPI_Receive(void)
{
	while(!(SPSR & (1<<SPIF)));     //Wait till reception is complete
	return(SPDR);     //Return received data
}
int mapp(float a, float in_min, float in_max, float out_min, float out_max)
{
	return (a - in_min) * (out_max - out_min)/(in_max - in_min) + out_min;
}
int x,y;
int xval,yval;
int main(void)
{
	DDRB |=(1<<PINB1) |(1<<PINB0);
	SPI_SlaveInit();
	pwm_set();
	int val = 0;
	while(1)
	{
		val = SPI_Receive();
		if((val & 0b00000001) == 0b00000001)
		{
			x = 4*val;
			x = mapp(x,0,1023,-255,255);
		}
		else if((val & 0b00000001) == 0b00000000)
		{
			y = 4*val;
			y = mapp(y,0,1023,-255,255);
		}
		if(x>230)
			x = 255;
		if(x<-230)
			x = -255;
		if(y>230)
			y = 255;
		if(y<-230)
			y = -255;
		if(abs(x)<80)
			x = 0;
		if(abs(y)<80)
			y = 0;
		
		if(abs(x)< 40 && abs(y)<40)	// no motion
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
			if(yval<20) yval = 0;
			OCR0 = x;
			OCR2 = yval;
		}
		
		else if(x>0 && y<0 && abs(y)>x)	//octet 2
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			xval = abs(255-x);
			if(xval<20) xval = 0;
			OCR0 = abs(y);
			OCR2 = xval;
		}
		
		else if(x<0 && y<0 && abs(y)>abs(x))	//octet 3
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<40)	xval = 0;
			OCR0 = xval;
			OCR2 = abs(y);
		}
		
		else if(x<0 && y<0 && abs(x)>=abs(y))	//octet 4
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval <40) yval = 0;
			OCR0 = yval;
			OCR2 = abs(x);
		}
		
		else if(x<0 && y>0 && abs(x)>y)	//octet 5
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			yval = abs(255-abs(y));
			if(yval<40)	yval = 0;
			OCR0 = abs(x);
			OCR2 = yval;
		}
		
		else if(x<0 && y>0 && y>=abs(x)) //octet 6
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<40)	xval = 0;
			OCR0 = y;
			OCR2 = xval;
		}
		
		else if(x>0 && y>0 && y>=abs(x))	//octet 7
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			xval = abs(255-abs(x));
			if(xval<20)	xval = 0;
			OCR0 = xval;
			OCR2 = y;
		}
		
		else if(x>0 && y>0 && x>y)	//octet 8
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			yval = abs(255-abs(y*1.25));
			if(yval<20)	yval = 0;
			OCR0 = yval;
			OCR2 = x;
		}
	}
}
/*
*/
