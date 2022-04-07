#ifndef DHT22_H_
#define DHT22_H_

#include <stdint.h>
#include <avr/io.h>

#define DHT22_DDRD DDRB
#define DHT22_PORT PORTB
#define DHT22_PIN PINB


typedef struct
{
	uint8_t pin; //[В,В,Т,Т,С]
	uint8_t data[5]; // 16bit Влажность + 16bit температура + 8bit проверочная сумма 
} dht22;

void dht22_init(dht22 *sensor, const uint8_t pin);

uint8_t dht22_get_bytes(dht22 *sensor);

uint8_t dht22_read_data(dht22 *sensor, float *temp, float *hum);

#endif  