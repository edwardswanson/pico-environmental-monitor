#pragma once

#include "../drivers/lcd_pcf8574.h"
#include "../drivers/led.h"
#include "pico/stdlib.h"

void ui_init(void);
void ui_startup(void);
void ui_update(float humidity, float temp);

