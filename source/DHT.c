#include "DHT.h"
#include <util/delay.h>

void dht22_init(dht22 *sensor, const uint8_t pin)
{
	if (sensor)
	{
		sensor->pin = pin;
		DHT22_DDRD |= (1<<sensor->pin); // Настройка пина выход
		DHT22_PORT |= (1<<sensor->pin); 
		_delay_ms(1000); //Ожидание инициализации
	}
}

uint8_t dht22_get_bytes(dht22 *sensor)
{
	if (!sensor)//Если не инициализирован
		return -1;
	
	uint8_t i = 0;
	uint8_t mask = 128;
	uint8_t count = 0;
	uint8_t byte = 0;
	uint8_t sum = 0;
	
	for(i = 5; i; --i)
		sensor->data[i-1] = 0;
	
	// Start signal
	DHT22_DDRD |= (1<<sensor->pin); 
	DHT22_PORT |= (1<<sensor->pin); //Фронт
	_delay_ms(1000);
	DHT22_PORT &= ~(1<<sensor->pin); //Спад
	_delay_ms(1);
	DHT22_PORT |= (1<<sensor->pin); 
	_delay_us(40);
	DHT22_DDRD &= ~(1<<sensor->pin); // Входной пин
	// Ответ
	while (!(DHT22_PIN & (1<<sensor->pin)));
	while ((DHT22_PIN & (1<<sensor->pin)));
	// Начало прередачи данных с сенсора
	for (i = 40; i; --i)
	{
		// Ожидание входящего бита
		while ( !(DHT22_PIN & (1<<sensor->pin)) );
		// Измерениие длинны бита
		while ((DHT22_PIN & (1<<sensor->pin)))
		++count;
		// Помещаем данные в память
		if (count > 4) 
		sensor->data[byte] |= mask;
		
		mask = mask >> 1;
		if (mask == 0)
		{
			mask = 128;
			byte++;
		}
		count = 0;
	}
	
	// Проверочная сумма
	for (i = 4; i; i--)
	sum += sensor->data[i-1];
	if ( (sum >> 8) != sensor->data[4] )
	return -1;

	// No Humidity error
	if (sensor->data[0])
		return 1;
		
	return -1;
}

uint8_t dht22_read_data(dht22 *sensor, float *temp, float *hum)
{
	uint8_t error_code = 0;
	if (!sensor)
		return -1;
	error_code = dht22_get_bytes(sensor);
	
	if (error_code < 1)
		return error_code;
	
	
	//Помещаем значения в переменные hum и temp
	*hum = (float)(sensor->data[0] << 8 | sensor->data[1]);
	*hum = (float)((*hum)/10);
	
	*temp = (float)(sensor->data[2] << 8 | sensor->data[3]);
	*temp = (float)((*temp)/10);
	
	return 1;
}