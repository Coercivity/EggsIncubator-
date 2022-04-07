/*
 * Incubator.c
 *
 * Created: 11/3/2021 2:52:51 PM
 * Author : Evgeny
 */ 

#define F_CPU 16000000UL

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "Button.h"
#include "DHT.h"
#include "Lamp.h"
#include "FAN.h"
#include "LCD.h"

 volatile int seconds = 0;
 int calculated_seconds = 0;
 int minutes = 0;
 int hours = 0;
 int days = 0;
 int DHT_time = 0;
 int DHT_delay_time = 10;//10секунд
 int Servo_time = 0;
 int Servo_delay_time = 10;//10секунд
 int temp_check_time = 0;
 int temp_check_delay_time = 5;//минута
 bool clockwise = true;
 bool isInitialized = false;
 bool cold_flag = false;
 bool hot_flag = false;

float desirableTemp = 38.0;
float temp = 0.0f;
float hum = 0.0f;
dht22 sensor;
uint8_t error_code = 0;

void DisplayDHT(float temp, float hum);
void timer_init()
{
	// инициализация Timer1
	cli();  // отключить глобальные прерывания
	TCCR1A = 0;   // установить регистры в 0
	TCCR1B = 0;

	OCR1A = 968; // установка регистра совпадения

	TCCR1B |= (1 << WGM12);  // включить CTC режим
	TCCR1B |= (1 << CS10); // Установить биты на коэффициент деления 1024
	TCCR1B |= (1 << CS12);

	TIMSK1 |= (1 << OCIE1A);  // включить прерывание по совпадению таймера
	sei(); // включить глобальные прерывания
	
}

void Reset()
{
	lcd_clrscr();
	lcd_puts("Incubation");
	lcd_gotoxy(1,3);
	lcd_puts("is ended");
	_delay_ms(10000);
	cli();
	cold_flag = false;
	hot_flag = false;
	Servo_time = 0;
	DHT_time = 0;
	temp_check_time = 0;
	seconds = 0;
	calculated_seconds = 0;
	minutes = 0;
	hours = 0;
	days = 0;
	temp = 0;
	hum = 0;
	lamp_off();
	FAN_off();
	timer_init();
}

void Setup()
{
	lcd_init(LCD_DISP_ON);
	button_init();
	FAN_init();
	dht22_init(&sensor, PINB0);
	lamp_init();
	timer_init();
	lamp_off();
	FAN_off();
}


void Servo()
{
	if(clockwise)
	{
		PORTB &= ~(1 << PINB2);
		PORTB |= (1 << PINB1);
	}
	else
	{
		PORTB &= ~(1 << PINB1);
		PORTB |= (1 << PINB2);
	}

	clockwise = !clockwise;
}

ISR(TIMER1_COMPA_vect)
{
	TCCR1B = 0;
	seconds++;
	TCCR1B |= (1 << CS10) | (1 << CS12) | (1 << WGM12); 
	
}



int main(void)
{
  	Setup();
	
	while(1)
	{	
		
		if(isResetButtonPressed())
			Reset();
		calculated_seconds = seconds;
		if(seconds >= 60)
		{
			calculated_seconds = 0;
			DHT_time = 0;
			temp_check_time = 0;
			Servo_time = 0;
			seconds = 0;
			minutes++;
			if(minutes == 60)
			{
				minutes = 0;
				hours++;
				if(hours == 24)
				{
					hours = 0;
					days++;
					if(days == 22)
					{
						lcd_clrscr();
						lcd_home();
						lcd_puts("Incubation is ended!");
						break;
					}
				}
			}
		}			
		
			
			if(days < 20)//Если период инкубации < 20 дней можно переворачивать
			{
				if(calculated_seconds - Servo_time > Servo_delay_time)//Для демонстарции секунды
				{
					Servo_time = calculated_seconds;
					Servo();
				}
			}
			
	
		
			if(calculated_seconds - DHT_time > DHT_delay_time)
			{
					DHT_time = calculated_seconds;
			}
				cli();
			error_code = dht22_read_data(&sensor, &temp, &hum);
			sei();
		if(isDisplayButtonPressed())
		{
			if ( error_code < 1 )
			{
				lcd_home();
				lcd_clrscr();
				lcd_puts("Reading DHT error");
				_delay_ms(100);
			}
			else
				DisplayDHT(temp, hum);	
		}
		else
		{
			lcd_home();
			lcd_clrscr();
			lcd_puts(" Incubation ");
			lcd_puts("Per:");
			lcd_gotoxy(3,1);
			char buffer[8] = "00000000";
			buffer[7] = days % 10;
			buffer[6] = (days / 10) % 10;
			buffer[5] = hours % 10;
			buffer[4] = (hours / 10) % 10;
			buffer[3] = minutes % 10;
			buffer[2] = (minutes / 10) % 10;
			buffer[1] = calculated_seconds % 10;
			buffer[0] = (calculated_seconds / 10) % 10;
			lcd_putc(buffer[6] + '0');
			lcd_putc(buffer[7] + '0');
			lcd_putc(':');
			lcd_putc(buffer[4] + '0');
			lcd_putc(buffer[5] + '0');
			lcd_putc(':');
			lcd_putc(buffer[2] + '0');
			lcd_putc(buffer[3] + '0');
			lcd_putc(':');
			lcd_putc(buffer[0] + '0');
			lcd_putc(buffer[1] + '0');




		}
		if(temp < (desirableTemp - 0.8))
		{
			FAN_on();
			lamp_on();
			if(calculated_seconds - temp_check_time > temp_check_delay_time && !cold_flag)//каждые 5 сек для демонстрации
			{
			
				temp_check_time = calculated_seconds;
				
				lcd_clrscr();
				lcd_gotoxy(3, 0);
				lcd_puts("TOO COLD!");
				lcd_gotoxy(2, 1);
				lcd_puts("LIGHT IS ON!");
				lcd_clrscr();
				lcd_gotoxy(3, 0);
				lcd_puts("TOO COLD!");
				lcd_gotoxy(2, 1);
				lcd_puts("LIGHT IS ON!");
				_delay_ms(100);
				cold_flag = true;
				hot_flag = false;
			}
		}
		
		if(temp > (desirableTemp + 1.5))
		{
			lamp_off();
			FAN_off();
			if(calculated_seconds - temp_check_time > temp_check_delay_time && !hot_flag)//каждые 5 сек для демонстрации
			{
			
				temp_check_time = calculated_seconds;
				lcd_clrscr();
				lcd_gotoxy(3, 0);
				lcd_puts("OVERHEAT!");
				lcd_gotoxy(2, 1);
				lcd_puts("LIGHT IS OFF!");
				_delay_ms(100);
				hot_flag = true;
				cold_flag = false;
			}
			
		}		
	}	
}

void DisplayDHT(float temp, float hum)
{
	temp *= 100;
	hum *= 100;
	int tt = (int)temp;
	int hh = (int)hum;
	char temp_buff[5];
	char hum_buff[5];
	int i = 4;
	while(i >= 0)
	{
		temp_buff[i] = tt % 10;
		tt /= 10;
		hum_buff[i--] = hh % 10;
		hh /= 10;
	}
		lcd_clrscr();
		lcd_home();
		lcd_puts("Temp:");
		for	(int i = 0; i < sizeof(temp_buff) / sizeof(char); i++)
		{
			if(temp_buff[i] == 0 && i == 0)
			continue;;
			if(i == 3)
			lcd_putc('.');
			lcd_putc(temp_buff[i] + '0');
			
		}
		lcd_putc('C');
		lcd_gotoxy(0, 1);
		lcd_puts("Hum:");
		for	(int i = 0; i < sizeof(hum_buff) / sizeof(char); i++)
		{
			if(hum_buff[i] == 0 && i == 0)
			continue;;
			if(i == 3)
			lcd_putc('.');
			lcd_putc(hum_buff[i] + '0');
			
		}
			lcd_putc('%');
}