/**
 * @file dht20.c
 * @brief DHT20 temperature and humidity sensor driver
 *
 */

#include "dht20.h"
#include "pico/stdlib.h"


uint8_t DHT20_start_commands[3] = {0xAC, 0x33, 0x00};


void dht20_init(void) {
    uint8_t soft_reset[] = {0xBA};  // from AHT20 docs
    i2c_write_blocking(DHT20_PORT, DHT20_ADDR, soft_reset, 1, false);
    sleep_ms(20);  // time required to soft reset does not exceed 20ms
}

int dht20_read(float *humidity, float *temp) {
    // send wakup command to the sensor
    i2c_write_blocking(DHT20_PORT, DHT20_ADDR, DHT20_start_commands, 3, false);
    sleep_ms(80);       // DHT20 needs 80ms

    // Response is 7 bytes:
        // first byte is status
        // next 20 bits is humidity
        // the following 20 bits is temp
        // last byte is CRC data
    uint8_t sensor_data[7];
    int result = i2c_read_blocking(DHT20_PORT, DHT20_ADDR, sensor_data, 7, false);

    // Extract the raw values
    // store in 32-bit variables
    // use bitshift to get them in the right place
    uint32_t humidity_raw_1 = sensor_data[1] << 12;
    uint32_t humidity_raw_2 = sensor_data[2] << 4;
    uint32_t humidity_raw_3 = sensor_data[3] >> 4;

    uint32_t humidity_raw_final = humidity_raw_1 | humidity_raw_2 | humidity_raw_3;

    // Same for temperature
    uint32_t temp_raw_1 = (sensor_data[3] & 0x0F) << 16;
    uint32_t temp_raw_2 = sensor_data[4] << 8;
    uint32_t temp_raw_3 = sensor_data[5];

    uint32_t temp_raw_final = temp_raw_1 | temp_raw_2 | temp_raw_3;


    // Convert to floats (full equation is from the docs)
    *humidity = ((float)humidity_raw_final / 1048576.0f) * 100.0f;
    *temp = (((float)temp_raw_final / 1048576.0f) * 200 - 50.0f);

    return 0;
}
