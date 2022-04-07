/*
 * Button.h
 *
 * Created: 11/3/2021 3:54:07 PM
 *  Author: Evgeny
 */ 
#include <avr/io.h>

#define BUTTON_PIN 0 //Пин для лампы (PC0)
#define BUTTON_RESET_PIN 3 //Пин для лампы (PC5)

void button_init()
{
    DDRC = DDRC & ~(1<<BUTTON_PIN); //Порт PC0(23)
	DDRC = DDRC & ~(1<<BUTTON_RESET_PIN); //Порт PC0(23)
}

bool isDisplayButtonPressed()
{
	if ((PINC & (1<<BUTTON_PIN)))
	{
		 _delay_ms(25);
		 if ((PINC & (1<<BUTTON_PIN))) 
			return true;
	}
	return false;
} 
bool isResetButtonPressed()
{
		if ((PINC & (1<<BUTTON_RESET_PIN)))
		{
			_delay_ms(25);
			if ((PINC & (1<<BUTTON_RESET_PIN)))
			return true;
		}
		return false;
}