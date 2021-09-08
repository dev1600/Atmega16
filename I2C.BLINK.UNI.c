/*
 * I2C.BLINK.UNI.c
 *
 * Created: 27-02-2020 10:42:23
 * Author : Apoorv
 */ 

//MASTER
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
void blink(int k)
{
	int i = 0;
	while(i < k)
	{
		PORTB |= (1<<PINB0);
		_delay_ms(100);
		PORTB &=~(1<<PINB0);
		_delay_ms(100);
		i++;
	}
}
void TWI_Master_Init()
{
	TWBR = 0b00000001;      
	TWSR &=~((1<<TWPS1)|(1<<TWPS0)); 
	TWCR |=(1<<TWEN);
	//blink(1);
}
void TWI_Start()
{												 //TWINT: Set to 1 when we complete our current event like Start,Stop,Transmit,Receive etc. 
	TWCR |=(1<<TWINT) |(1<<TWEN) |(1<<TWSTA);	 //TWEN: TWI enable bit, takes control over the I/O pins  
	while(!(TWCR & (1<<TWINT)));				 //TWSTA: TWI Start condition bit
	while((TWSR & (0xF8)) != 0x08);
	//blink(1);			     //TWSR: TWI Status Register, 0x08 is to send start condition and wait for Ack
}			   									 //TWCR: TWI Control Register												 
void TWI_Addr(unsigned char Addr)
{
	//blink(2);
	TWDR = Addr;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	while((TWSR & 0xF8) != 0x18); 
	blink(2);    //Check for Acknowledgment; 0x18 is the Ack for Address Transfer
}
void TWI_Write_Data(char Data)
{
	TWDR = Data;								 //TWDR: TWI Address Register
	TWCR =(1<<TWINT)|(1<<TWEN);     //Start Transmission Of Data
	while(!(TWCR & (1<<TWINT)));     //Transfer Data
	while((TWSR & 0xF8) != 0x28);     //Check for Acknowledgment; 0x28 is the Ack for Data Transfer    
}
void TWI_Stop()
{												 //TWSTO: TWI End condition bit
	TWCR = (1<<TWINT) |(1<<TWEN) |(1<<TWSTO);     //Transmit Stop Condition 
	while(TWCR & (1<<TWSTO));  // Wait till stop condition is transmitted
}
int main(void)
{
	DDRB &=~(1<<PINB2);
	DDRB |=(1<<PINB0);
	PORTB |=(1<<PINB2);
	TWI_Master_Init();
	int pin;
    while (1) 
    {
		//blink(1);
		TWI_Start();
		TWI_Addr(0x20); 
		pin = PINB&(1<<2);
		if(pin)
		{
			TWI_Write_Data(0b00011111);
			//blink(1);
		}
		else
		{
			TWI_Write_Data(0b11000000);
		}
		TWI_Stop();
    }
}
/*
*/























//SLAVE
/*
#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
void blink(int k)
{
	int i = 0;
	while(i<k)
	{
		PORTB |=(1<<PINB0);
		_delay_ms(100);
		PORTB &=~(1<<PINB0);
		_delay_ms(100);
		i++;
	}
}
void TWI_Slave_Init(unsigned char Slave_Addr)
{
	TWAR = Slave_Addr;     //TWAR: Slave Address
}
void TWI_Read_Data()
{
	int dat;
	TWCR = (1<<TWINT) |(1<<TWEA) |(1<<TWEN);     //TWEA: Generate Acknowledge Bit
	while(!(TWCR & (1<<TWINT)));			 //TWEA: TWI Enable Acknowledge Bit
	while((TWSR & 0xF8) != 0x80);
	dat = TWDR;
	if(dat == 0b00011111)
		blink(3);
}
void TWI_Match_ACK()
{
	while((TWSR & 0xF8) != 0x60)
	{
		TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);     
		while(!(TWCR &(1<<TWINT)));
	}
}
int main()
{
	DDRB |=(1<<PINB0);
	TWI_Slave_Init(0x20);
	while(1)
	{
		TWI_Match_ACK();
		TWI_Read_Data();
	}
}
/*
*/









