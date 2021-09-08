/*
 * UART.BLINK.UNI.c
 *
 * Created: 03-02-2020 17:27:48
 * Author : apoor
 */ 
//RECEIVE
#include <avr/io.h>
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
unsigned char USART_Receive(void)
{
	//Wait for data to be received
	while(!(UCSRA & (1<<RXC)));
	//return received data
	return UDR;
}

int main(void)
{
	DDRB |=(1<<PINB1);
	UART_Init();
	//int val ;
	while(1)
	{
		if(USART_Receive()==5)
			PORTB |=(1<<PINB1);
		else if(USART_Receive()==0)
			PORTB&=~(1<<PINB1);
	}
}
/*
*/






//TRANSMIT
#include <avr/io.h>
void UART_Init(void)
{
	//set baud rate
	unsigned int ubrr = 25;
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	//enable tx
	UCSRB |=(1<<TXEN) |(1<<RXEN);
	//set format
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit(unsigned char data)
{
	//wait for empty transmit buffer
	while(!(UCSRA & (1<<UDRE)));
	UDR = data;
}

int main(void)
{
	DDRB&=~(1<<PINB4);
	PORTB|=(1<<PINB4);
	UART_Init();
	while(1)
	{
		if(PINB & (1<<PINB4))
			USART_Transmit(5);
		else 
			USART_Transmit(0);
	}
}
/*
*/