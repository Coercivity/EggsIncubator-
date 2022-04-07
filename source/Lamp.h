/*
 * Lamp.h
 *
 * Created: 11/3/2021 5:18:43 PM
 *  Author: Evgeny
 */ 

#include <avr/io.h>

#define LAMP_PIN 2 //Пин для лампы (PC1)


void lamp_init()
{
	DDRC = DDRC | (1<<LAMP_PIN);
}
void lamp_off()
{
	PORTC = PORTC &  ~(1<<LAMP_PIN);
}
void lamp_on()
{
	PORTC = PORTC | (1<<LAMP_PIN);	
}