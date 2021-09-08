/*
 * UART.BLINK.BI.c
 *
 * Created: 03-02-2020 17:27:48
 * Author : apoor
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

void UART_Init(void)
{
	//set baud rate
	unsigned int ubrr = 25;
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//enable rx
	UCSRB |=(1<<RXEN) |(1<<TXEN);
	//set format
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}
void UART_Transmit(unsigned char data)
{
	//wait for empty transmit buffer
	while(!(UCSRA & (1<<UDRE)));
	UDR = data;
}
void blink(int n)
{
	int i=0;
	while(i<n)
	{
		PORTB|=(1<<PINB1);
		_delay_ms(100);
		PORTB &=~(1<<PINB1);
		_delay_ms(100);
		i++;
	}
}
void UART_Receive(void)
{
	int flag;
	//Wait for data to be received
	while(!(UCSRA & (1<<RXC)));
	//return received data
	 flag = UDR;
	if(flag==10)
		blink(3);
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              

int main(void)
{
	DDRB |=(1<<PINB1);
	DDRB &=~(1<<PINB2);
	PORTB |=(1<<PINB2);
	UART_Init();
	//int val ;
	while(1)
	{
			
		if(PINB & (1<<PB2))
		{
			blink(3);
			UART_Transmit(10);
		}
		else
			UART_Transmit(5);
			
		UART_Receive();
	}
}