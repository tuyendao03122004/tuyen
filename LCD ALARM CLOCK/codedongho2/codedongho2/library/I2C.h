#include <avr/io.h>
#define F_CPU 11059200
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define BITRATE(TWSR)	((F_CPU/SCL_CLK)-16)/(2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1)))))
#define Slave_Write_Address	0x20
#define Slave_Read_Address	0x21
#define SCL_CLK 100000L

#define Device_write_address	0xD0
#define Device_Read_address		0xD1
#define TimeFormat12			0x40
#define AMPM					0x20

void I2C_Init(void);
uint8_t I2C_Start(char write_address);
uint8_t I2C_Repeated_Start(char read_address);
void I2C_Start_Wait(char write_address);
uint8_t I2C_Write(char data);
int I2C_Read_Ack(void);
int I2C_Read_NACK(void);
void I2C_Stop(void);

void I2C_Init()
{
	TWBR = BITRATE(TWSR = 0X00);
}

uint8_t I2C_Start(char write_address)
{
	uint8_t status;
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	status = TWSR & 0xF8;
	if(status != 0x08)
	return 0;
	TWDR = write_address;
	TWCR = (1<<TWEN)|(1<< TWINT);
	while (!(TWCR & (1<<TWINT)));
	status = TWSR & 0xF8;
	if (status == 0x18)
	return 1;
	if (status == 0x20)
	return 2;
	else
	return 3;
}

uint8_t I2C_Repeated_Start(char read_address)
{
	uint8_t status;
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	status = TWSR & 0xF8;
	if (status != 0x10)
	return 0;
	TWDR = read_address;
	TWCR = (1<<TWEN)|(1<< TWINT);
	while (!(TWCR & (1<<TWINT)));
	status = TWSR & 0xF8;
	if (status == 0x40)
	return 1;
	if (status == 0x20)
	return 2;
	else
	return 3;
}

void I2C_Stop()
{
	TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
	while (TWCR & (1<<TWSTO));
}

void I2C_Start_Wait(char write_address)
{
	uint8_t status;
	while (1)
	{
		TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
		while  (!(TWCR & (1<<TWINT)));
		status = TWSR & 0xF8;
		if (status !=0x08)
		continue;
		TWDR = write_address;
		TWCR = (1<<TWEN)|(1<< TWINT);
		while (!(TWCR & (1<<TWINT)));
		status = TWSR & 0xF8;
		if (status != 0x18)
		{
			I2C_Stop();
			continue;
		}
		break;
	}
}

uint8_t I2C_Write(char data)
{
	uint8_t status;
	TWDR = data;
	TWCR = (1<<TWEN)|(1<< TWINT);
	while (!(TWCR & (1<<TWINT)));
	status = TWSR & 0xF8;
	if (status == 0x28)
	return 0;
	if (status == 0x30)
	return 1;
	else
	return 2;
}

int I2C_Read_Ack()
{
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	return TWDR;
}

int I2C_Read_NACK()
{
	TWCR = (1<< TWEN)|(1<<TWINT);
	while (!(TWCR & (1<< TWINT)));
	return TWDR;
}