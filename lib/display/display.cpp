#include <stdio.h>
#include "pico/stdlib.h"
#include "display.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "../config.hpp"

#define DISPLAY_ADDRESS 0x3C //0x78か0x3C

void DisplaySetup(){
    gpio_set_function(DisplaySDApin, GPIO_FUNC_I2C);
    gpio_set_function(DisplaySCLpin, GPIO_FUNC_I2C);
    gpio_pull_up(DisplaySCLpin);
    gpio_pull_up(DisplaySDApin);
}
