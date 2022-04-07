/*
 * FAN.h
 *
 * Created: 11/3/2021 5:29:58 PM
 *  Author: Evgeny
 */ 

#include <avr/io.h>

#define FAN_PIN 1 //Пин для вентилятора (PC2)


void FAN_init()
{
	DDRC = DDRC | (1<<FAN_PIN);
}
void FAN_off()
{
	PORTC = PORTC &  ~(1<<FAN_PIN);
}
void FAN_on()
{
	PORTC = PORTC | (1<<FAN_PIN);
}