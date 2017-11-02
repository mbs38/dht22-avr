/* Fast DHT Lirary
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com).
 * Modified in 2017 by Max Brueggemann.
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * Original library written by Adafruit Industries. MIT license.
 */

#include "dht22.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define DHT_COUNT 6
#define DHT_MAXTIMINGS 85

uint8_t dhtStates = 0;

uint8_t dht_read(uint8_t pin, int16_t *temp, int16_t *hum)
{
    uint8_t tmp;
    uint8_t sum = 0;
    uint8_t j = 0, i;
    uint8_t last_state = 1;
    uint8_t counter = 0;
	uint8_t data[] = {0,0,0,0,0};
 
    /* Now pull it low for ~20 milliseconds */
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		DDR_DHT |= (1 << pin);
		PORT_DHT &= ~(1 << pin);
	}
    _delay_ms(2); //MB: changed this from 20 to 2ms, datasheet states that 1ms is enough
    ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		PORT_DHT |= (1 << pin);
		_delay_us(40);
		DDR_DHT &= ~(1 << pin);

		/* Read the timings */
		for (i = 0; i < DHT_MAXTIMINGS; i++) {
			counter = 0;
			while (1) {
				tmp = ((PIN_DHT & (1 << pin)) >> 1);
				_delay_us(3);

				if (tmp != last_state)
				break;

				counter++;
				_delay_us(1);

				if (counter == 40) //MB: let's be a bit more optimistic here...
				break;
			}

			last_state = ((PIN_DHT & (1 << pin)) >> 1);

			if (counter == 40)
			break;

			/* Ignore first 3 transitions */
			if ((i >= 4) && (i % 2 == 0)) {
				/* Shove each bit into the storage bytes */
				data[j/8] <<= 1;
				if (counter > DHT_COUNT)
				data[j/8] |= 1;
				j++;
			}
		}
	}
    sum = data[0] + data[1] + data[2] + data[3];

    if ((j >= 40) && (data[4] == (sum & 0xFF)))
	{
		*temp = data[2] & 0x7F;
		*temp *= 256;
		*temp += data[3];

		if (data[2] & 0x80)
		*temp *= -1;

		/* Reading humidity */
		*hum = data[0];
		*hum *= 256;
		*hum += data[1];
        return 1;
	} 
	else {
		*hum=-9999;
		*temp=-9999;
	}
    return 0;
}

void dht22Start(void)
{
	if(dhtStates==0) dhtStates=1;
}


/*
* Call twice per second
*/
uint8_t dht_state_machine(int16_t *dht22Output)
{
	switch (dhtStates)
	{
		case 1:	//init, pull pins high & wait
		{
			ATOMIC_BLOCK(ATOMIC_FORCEON)
			{
				DDR_DHT |= (1 << PINNR_DHT0)|(1 << PINNR_DHT1);
				PORT_DHT |= (1 << PINNR_DHT0)|(1 << PINNR_DHT1);
			}
			dhtStates++;
		}
		break;
		case 2: //read first sensor
		{
			dht_read(PINNR_DHT0, dht22Output, dht22Output+1);	
			dhtStates++;
		}
	    break;
		case 3: //read second sensor
		{
			dht_read(PINNR_DHT1, dht22Output+2, dht22Output+3);
			dhtStates=0;
			return 1;
		}
		break;
	}
	return 0;
}
