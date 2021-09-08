/*
 * UART.MC10BIT.c
 *
 * Created: 03-02-2020 19:51:07
 * Author : apoor
 */ 
//TRANSMIT
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
void UART_Init(void)
{
	//set baud rate
	unsigned int ubrr = 25;
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//enable tx
	UCSRB |=(1<<TXEN);
	//set format
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void UART_Transmit(unsigned char data)
{
	//wait for empty transmit buffer
	while(!(UCSRA & (1<<UDRE)));
	UDR = data;
}

int main(void)
{
	UART_Init();
	DDRA = 0b00000000;
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS1) |(1<<ADPS0); //set prescaler: /8
	ADMUX &=~(1<<ADLAR);
	ADCSRA |=1<<ADIE;
	//ADMUX = 0b010000000;
	ADCSRA |=1<<ADSC;
	int x=0,y=0,x2=0,y2=0;
	//int ten;
	//int x=0,y=0;
	//uint16_t ten=0;
	//uint8_t x=0,y=0,x2=0,y2=0;
	//int xval,yval,x,y;
	while(1)
	{
		uint8_t low = ADCL;
		uint16_t high = ADCH<<8 | low;
		switch (ADMUX)
		{
			case 0b01000000:
				//ten = adc_read(0);
				//x = high;
				x = (high | 0b00000001);
				//x = (xval | 0b00000001);
				x2 = high>>8;
				UART_Transmit(x);
				UART_Transmit(x2);
				ADMUX = 0b01000001;
				break;
			case 0b01000001:
				//ten = adc_read(1);
				//yv = high/4;
				y = (high & 0b11111110);
				//y = (yval & 0b11111110);
				y2 = high>>8;
				UART_Transmit(y);
				UART_Transmit(y2);
				ADMUX = 0b01000000;
				break;
		}
		ADCSRA |= 1<<ADSC;
	}
}









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
void UART_Init(void)
{
	//set baud rate
	unsigned int ubrr = 25;
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//enable rx
	UCSRB |=(1<<RXEN);
	//set format
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
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

unsigned char UART_Receive(void)
{
	//Wait for data to be received
	while(!(UCSRA & (1<<RXC)));
	//return received data
	return UDR;
}
int mapp(float a, float in_min, float in_max, float out_min, float out_max)
{
	return (a - in_min) * (out_max - out_min)/(in_max - in_min) + out_min;
}
int x=0,y=0;
//uint8_t x2=0,y2=0;
int xval=0,yval=0;
int main(void)
{
	DDRB |=(1<<PINB1) |(1<<PINB0);
	UART_Init();
	pwm_set();
	//PORTB |=(1<<PINB0) |(1<<PINB1);
	uint16_t val=0;
	int low=0,high=0;
	int buffer = 35;
	while(1)
	{
		
		low = UART_Receive();
		high = UART_Receive();
		val = high<<8 | low;
		//val = UART_Receive();
		if((val & 0b00000001) == 0b00000001)
		{
			//x = val*4;
			x = mapp(val,0,1023,-255,255);
		}
		else if((val & 0b00000000) == 0b00000000)
		{
			//y = val*4;
			y = mapp(val,0,1023,-255,255);
		}
		if(abs(x)<80)
		x = 0;
		if(abs(y)<80)
		y = 0;
		
		if(x>230)
		x = 255;
		else if(x<-230)
		x = -255;
		
		if(y>230)
		y = 255;
		else if(y<-230)
		y = -255;
		
		if(abs(x)< 40 && abs(y)<40) // no motion
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
			yval = abs(255-abs(y*1.25));
			if(yval<buffer)	yval = 0;
			OCR0 = yval;
			OCR2 = x;
		}
	}
}