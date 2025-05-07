#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

void send_a_command(unsigned char command);
void send_a_character(unsigned char character);
void send_a_string(char *string_of_characters);

#define EN PD4
#define RS PD6
#define RW PD5

#define LCD_Data_Dir DDRB
#define LCD_Command_Dir DDRD
#define LCD_Data_Port PORTB
#define LCD_Command_Port PORTD

void send_a_command(unsigned char command)
{
	LCD_Command_Port &= ~(1 << RW);			// RW=0
	LCD_Command_Port &= ~(1 << RS);			// RS=0, ghi lenh
	LCD_Data_Port = command;
	LCD_Command_Port |= (1 << EN);			// LCDCommand PORTB,,,,,, LCDData PORTA
	_delay_ms(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
}

void send_a_character(unsigned char character)
{
	LCD_Command_Port &= ~(1 << RW);			// RW=0
	LCD_Command_Port |= (1 << RS);			// RS=1, ghi du lieu
	LCD_Data_Port = character ;
	LCD_Command_Port |= (1 << EN);
	_delay_ms(1);
	LCD_Command_Port &= ~(1 << EN);
	_delay_ms(1);
	
}

void send_a_string(char *string_of_characters)
{
	int i;
	for (i=0; string_of_characters[i]!=0;i++)
	{
		send_a_character(string_of_characters[i]);
	}
}