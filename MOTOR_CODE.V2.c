/*
 * MOTOR_CODE.V2.c
 *
 * Created: 06-03-2020 14:46:44
 * Author : Apoorv
 */ 

//Let OCR0 be BL
//Let OCR2 be BR
//Let PB0 be FL
//Let PB1 be FR
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
int mapp(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min)/(in_max - in_min) + out_min;
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
void Drive(int oct0,int oct2, int a, int b, int p, int q, int X, int Y)
{
	OCR0 = abs(255*oct0 -X*a - Y*b);
	OCR2 = abs(255*oct2 -X*p - Y*q);
}
int adc_read( unsigned char dig)
{
	dig = dig & 0b00000111;
	ADMUX = dig; //declaring pin
	ADMUX |=(1<<REFS0);
	ADMUX &=~(1<<ADLAR);
	ADCSRA |=(1<<ADSC); //start conversion
	while((ADCSRA & (1<<ADSC))); //wait for conversion;
	uint8_t low = ADCL;
	uint16_t high = ADCH<<8 | low;
	return (high);
}
int x,y,xval,yval;
int main(void)
{
	DDRA = 0b00000000;
	ADCSRA = (1<<ADEN)|(1<<ADPS2); //set prescaler
	pwm_set();
	DDRB |=(1<<PINB0) |(1<<PINB1);
	while (1)
	{
		x = adc_read(0);
		y = adc_read(1);
		x = mapp(x,0,1023,-255,255);
		y = mapp(y,0,1023,-255,255);
		
		if(x>230)
			x = 255;
		if(x<-230)
			x = -255;
		if(y>230)
			y = 255;
		if(y<-230)
			y = -255;
		if(abs(x)<20)
			x = 0;
		if(abs(y)<20)
			y = 0;
		
		if(abs(x)< 30 && abs(y)<30) // no motion
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			Drive(0,0,0,0,0,0,0,0);
		} 
		
		else if(abs(x) < 10 && y < 0) //forward
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			Drive(0,0,0,1,0,1,0,y);
		}
		
		else if(abs(x) < 10 && y > 0) //backward
		{
			PORTB &=~(1<<PINB0);
			PORTB &=~(1<<PINB1);
			Drive(0,0,0,1,0,1,x,y);
		}
		
		else if(abs(y)<20 && x > 0) //spot turn right
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			Drive(0,0,1,0,1,0,x,0);
		}
		
		else if(abs(y)<20 && x < 0) //spot turn left
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			Drive(0,0,1,0,1,0,x,0);
		}
		
		else if(x>0 && y <0 && x>=abs(y))	//octet 1
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			Drive(1,0,0,-1,1,0,x,y);
		}
		
		else if(x>0 && y<0 && abs(y)>x)	//octet 2
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			Drive(0,1,0,1,1,0,x,y);
		}
		
		else if(x<0 && y<0 && abs(y)>abs(x))	//octet 3
		{
			PORTB |=(1<<PINB0) |(1<<PINB1);
			Drive(1,0,-1,0,0,1,x,y);
		}
		
		else if(x<0 && y<0 && abs(x)>=abs(y))	//octet 4
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			Drive(1,0,0,-1,1,0,x,y);
		}
		
		else if(x<0 && y>0 && abs(x)>y)	//octet 5
		{
			PORTB &=~(1<<PINB0);
			PORTB |=(1<<PINB1);
			Drive(0,1,1,0,0,-1,x,y);
		}
		
		else if(x<0 && y>0 && y>=abs(x)) //octet 6
		{
			PORTB &=~((1<<PINB0) |(1<<PINB1));
			Drive(0,1,0,1,-1,0,x,y);
		}
		
		else if(x>0 && y>0 && y>=abs(x))	//octet 7
		{
			PORTB &=~((1<<PINB0) |(1<<PINB1));
			Drive(1,0,1,0,0,1,x,y);
		}
		
		else if(x>0 && y>0 && x>y)	//octet 8
		{
			PORTB |=(1<<PINB0);
			PORTB &=~(1<<PINB1);
			Drive(1,0,0,1,1,0,x,y);
		}
	}
}



