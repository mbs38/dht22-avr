/* struct dht22 AVR Lirary
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 * Modified in 2017 by Max Brueggemann.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by Adafruit Industries. MIT license.
 */

#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>

/*
 * Sensor's port and pin
 */
#define DDR_DHT DDRC
#define PORT_DHT PORTC
#define PIN_DHT PINC
#define PINNR_DHT0 5
#define PINNR_DHT1 4

void dht22Start(void);

uint8_t dht_state_machine(int16_t *dht22Output);

#endif
