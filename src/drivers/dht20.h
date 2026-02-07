/**
 * @file dht20.h
 * @brief DHT20 temperature and humidity sensor driver
 *
 * DATASHEETS
 * https://cdn.sparkfun.com/assets/8/a/1/5/0/DHT20.pdf
 * https://files.seeedstudio.com/wiki/Grove-AHT20_I2C_Industrial_Grade_Temperature_and_Humidity_Sensor/AHT20-datasheet-2020-4-16.pdf
 */

#pragma once
#include "hardware/i2c.h"

#define DHT20_ADDR 0x38
#define DHT20_PORT i2c0


void dht20_init(void);
int dht20_read(float *humidity, float *temp);
