#pragma once
#include "hardware/i2c.h"

#define DHT20_ADDR 0x38
#define DHT20_PORT i2c0


void dht20_init(void);
int dht20_read(float *humidity, float *temp);