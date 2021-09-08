/*
 * ADC_Polling.c
 *
 * Created: 26-01-2020 16:18:40
 * Author : apoor
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
void pwm_set()
{
	TCCR0 |= (1<<WGM00) |(1<<WGM01); //fast pwm
	TCCR0 |= (1<<COM01) |(1<<CS00);  //OC0 and prescaler set
	TCCR2 |= (1<<WGM20) |(1<<WGM21); //fast pwm
	TCCR2 |= (1<<COM21) |(1<<CS20);  //Oc2 and prescaler set
	DDRB |= 1<<PINB3;
	DDRD |= 1<<PIND7;
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
unsigned int x,y;
int main(void)
{
	DDRA = 0b00000000;
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2); //set prescaler
	pwm_set();
	while (1)
	{
		x = adc_read(0);
		y = adc_read(1);
		OCR0 = x/4;
		OCR2 = y/4;
	}
}
