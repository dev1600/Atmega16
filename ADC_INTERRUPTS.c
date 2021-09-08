/*
 * ADC_Interrupts.c
 *
 * Created: 26-01-2020 18:38:57
 * Author : apoor
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
unsigned int x,y;
void pwm_set()
{
		TCCR0 |= (1<<WGM00) |(1<<WGM01) |(1<<COM01) |(1<<CS00);
		TCCR2 |= (1<<WGM20) |(1<<WGM20) |(1<<COM21) |(1<<CS20);
		DDRB |= 1<<PINB3;
		DDRD |= 1<<PIND7;
}
/*int adc_0()
{
		ADMUX = 0b01000000;
		ADCSRA |=(1<<ADSC); //start conversion
		while((ADCSRA & (1<<ADSC))); //wait for conversion;
		return(ADC);
}
int adc_1()
{
		ADMUX = 0b01000001;
		ADCSRA |=(1<<ADSC);
		while((ADCSRA & (1<<ADSC))); //wait for conversion;
		return(ADC);
}
int adc( unsigned char dig)
{
	dig = dig & 0b00000111;
	ADMUX = dig; //declaring pin
	ADCSRA |=(1<<ADSC); //start conversion
	while((ADCSRA & (1<<ADSC))); //wait for conversion;
	//ADCSRA |=(1<<ADIF); // rewrite to set 1
	return (ADC);
}*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned int x = 0;
unsigned int y = 0;
int main(void)
{
	DDRA = 0b00000000;
	pwm_set();
	ADMUX=(1<<REFS0);
	ADCSRA|=(1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);
	//ADMUX&=~(1<<ADLAR);
	ADCSRA |= 1<<ADIE;
	sei();
	ADCSRA |= 1<<ADSC;
	while (1)
	{
		OCR0 = x;
		OCR2 = y;
	}
}
ISR(ADC_vect)
{
	uint8_t low = ADCL;
	uint16_t high = ADCH<<8 | low;
	switch (ADMUX)
	{
		case 0b01000000:
		if((high/4)<50)
		x = 0;
		else
		x = high/4;
		ADMUX = 0b01000001;
		break;
		case 0b01000001:
		if((high/4)<50)
		y=0;
		else
		y = high/4;
		ADMUX = 0b01000000;
		break;
	}
	ADCSRA |= 1<<ADSC;
}