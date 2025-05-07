
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
#include <LCD.h>
#include <I2C.h>

int second,minute,hour, day, date, month, year;
int seconds,minutes,hours,dates,dayss,months,years;
int ALhour,ALMIN,ALH,ALM;

char tmp[2];

uint8_t BCDToDecimal(uint8_t bcd) {
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Convert decimal to BCD
uint8_t decimalToBCD(uint8_t decimal) {
	return ((decimal / 10) << 4) | (decimal % 10);
}

void RTC_WRITE_CALENDER (char WRITE_CLOCK_CALENDER, char years, char months, char dates,char dayss)
{
	years  = decimalToBCD(years);
	months = decimalToBCD(months);
	dayss  = decimalToBCD(dayss);
	dates  = decimalToBCD(dates);
	
	I2C_Start(Device_write_address);
	I2C_Write(WRITE_CLOCK_CALENDER);
	
	I2C_Write(dayss);
	I2C_Write(dates);
	I2C_Write(months);
	I2C_Write(years);
	I2C_Stop();
	
}

void RTC_WRITE_CLOCK (char WRITE_CLOCK_ADDRESS, char hours, char minutes, char seconds)
{

	hours = decimalToBCD(hours);
	minutes = decimalToBCD(minutes);
	seconds = decimalToBCD(seconds);
	I2C_Start(Device_write_address);
	I2C_Write(WRITE_CLOCK_ADDRESS);
	
	I2C_Write(seconds);
	I2C_Write(minutes);
	I2C_Write(hours);
	
	I2C_Stop();
}

void RTC_READ_CLOCK (char READ_CLOCK_ADDRESS)
{
	I2C_Start(Device_write_address);
	I2C_Write(READ_CLOCK_ADDRESS);
	I2C_Repeated_Start(Device_Read_address);
	
	second = I2C_Read_Ack();
	minute = I2C_Read_Ack();
	hour   = I2C_Read_NACK();
	I2C_Stop();
}

void RTC_READ_CALENDER( char READ_CALENDER_ADDRESS)
{
	I2C_Start(Device_write_address);
	I2C_Write(READ_CALENDER_ADDRESS);
	I2C_Repeated_Start(Device_Read_address);
	day   = I2C_Read_Ack();
	date  = I2C_Read_Ack();
	month = I2C_Read_Ack();
	year  = I2C_Read_NACK();
	I2C_Stop();
}

void adjust_value(char *label, int *value, int max_value, unsigned char position) {
	char temp[10];
	send_a_command(0x01);
	_delay_ms(1);
	send_a_command(0x80);
	send_a_string(label);
	_delay_ms(1);

	while (1) {
		send_a_command(position);
		_delay_ms(1);
		sprintf(temp, "%02d", *value);
		send_a_string(temp);
		_delay_ms(1);

		if (bit_is_set(PINA, 1)) {
			if (*value < max_value) {
				(*value)++;
				} else {
				*value = 0;
			}
			_delay_ms(10);
		}

		if (bit_is_set(PINA, 2)) {
			if (*value > 0) {
				(*value)--;
				} else {
				*value = max_value;
			}
			_delay_ms(6);
		}

		if (bit_is_set(PINA, 0)) break;
	}
}

int NHUANKH(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int MAXDATE(int month, int year) {
	if (month == 2)
	{ // Tháng 2
		if (is_leap_year(year)) return 29;
		
		else return 28;
	}
	else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) return 31;
	else return 30;
}


int main(void)

{	
	char temp [10];
	char buffer[20];
	char* days[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	I2C_Init();
	DDRA = 0xFF;
	LCD_Data_Dir = 0xFF;
	LCD_Command_Dir |= (1 << RS)| (1 << RW) | (1 << EN);										// setting compare value equal to counter clock frequency to get an interrupt every second
	send_a_command(0x01);
	_delay_ms(1);
	send_a_command(0x38);
	_delay_ms(1);
	send_a_command(0x0C);
	_delay_ms(1);
	
	send_a_string("BTL TKHTN NHOM17");
	_delay_ms(250);
	send_a_command(0x01);
	
	START:
	send_a_command(0x80);
	//format 24h
	RTC_READ_CALENDER(3);
	_delay_ms(1);
	sprintf(buffer,"%02x/%02x/%02x %3s ", date, month, year, days[day-1] );
	send_a_string(buffer);
	_delay_ms(1);
	
	while (1)
	{
		if(hour>23)
		{
			send_a_command(0x80);
			RTC_READ_CALENDER(3);
			_delay_ms(1);
			sprintf(buffer,"%02x/%02x/%02x %3s ", date, month, year, days[day-1] );
			send_a_string(buffer);
			_delay_ms(1);
		}
		RTC_READ_CLOCK(0);
		_delay_ms(1);
		sprintf(buffer, "%02x:%02x:%02x ",(hour & 0b00111111), minute, second);
		send_a_command(0xC0);	// dua con tro toi vi tri thu 4
		send_a_string(buffer);		// gui chuoi ki tu ra LCD
		_delay_ms(1);
		send_a_command(0xC0 + 0x09);
		if (bit_is_set(PINA,5))
		{
			send_a_string(" ON ");
			if(bit_is_set(PINA,3)&&(bit_is_clear(PINA,7)))
			{
				ALMIN = 0;
				ALhour = 0;
			}
			if ((decimalToBCD(ALhour) == hour) && (decimalToBCD(ALMIN) == minute)) PORTA &= (0 << PINA7);
			else PORTA |= (1 << PINA7);
		}
		else
		{
			send_a_string("OFF");
			PORTA |= (1 << PINA7);
		}
		if((bit_is_set(PINA,4))&&(bit_is_set(PINA,0)))
		{
			seconds = BCDToDecimal(second);
			minutes = BCDToDecimal(minute);
			hours   = BCDToDecimal(hour);
			dayss	= BCDToDecimal(day);
			dates	= BCDToDecimal(date);
			months  = BCDToDecimal(month);
			years	= BCDToDecimal(year);
			adjust_value("hour: ", &hours, 23,0x85);
			adjust_value("minute: ", &minutes, 59,0x87);
			adjust_value("second: ", &seconds, 59,0x87);
			adjust_value("year: ", &years, 100,0x85);
			adjust_value("month: ", &months, 12,0x86);
			adjust_value("date: ", &dates, MAXDATE(years,months),0x85);
			send_a_command(0x01);
			send_a_command(0x80);
			send_a_string("thu: ");
			while(1)
			{
				send_a_command(0x84);
				_delay_ms(1);
				sprintf(temp,"%02s", days[dayss-1]);
				send_a_string(temp);

				if (bit_is_set(PINA, 1))
				{
					if (dayss < 7) dayss++;  
					else  dayss = 0; 
	
				}
				if (bit_is_set(PINA,2))  
				{
					if (dayss > 0) dayss--;  

					else dayss = 6;  
				}
				if(bit_is_set(PINA,0)) break;
			}
			send_a_command(0x01);
			RTC_WRITE_CLOCK(0,hours,minutes,seconds);
			RTC_WRITE_CALENDER(3,years,months,dates,dayss);
			goto START;
		}
		if((bit_is_clear(PINA,4))&&(bit_is_set(PINA,0)))
		{
			ALH = BCDToDecimal(hour);
			adjust_value("ALHOUR: ", &ALH, 23,0x87);
			ALhour = ALH;
			
			ALM = BCDToDecimal(minute);
			adjust_value("ALminute: ",&ALM,59,0x89);
			ALMIN = ALM;
			send_a_command(0x01);
			goto START;
		}
	}
}