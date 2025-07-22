
// u8g2_user_impl.cpp
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#include "display.hpp"
#include "../config.hpp"
#include "u8g2.h"

// I2C: GP4 = SDA, GP5 = SCL, I2C0使用
uint8_t u8x8_byte_pico_i2c(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_BYTE_INIT:
            i2c_init(DisplayI2C, 400 * 1000);  // 400kHz
            gpio_set_function(DisplaySDApin, GPIO_FUNC_I2C);  // SDA
            gpio_set_function(DisplaySCLpin, GPIO_FUNC_I2C);  // SCL
            gpio_pull_up(DisplaySDApin);
            gpio_pull_up(DisplaySCLpin);
            break;

        case U8X8_MSG_BYTE_SEND:
            i2c_write_blocking(DisplayI2C, 0x3C, (uint8_t *)arg_ptr, arg_int, true);
            break;

        case U8X8_MSG_BYTE_START_TRANSFER:
        case U8X8_MSG_BYTE_END_TRANSFER:
            break;
    }
    return 1;
}

// GPIOコールバック（未使用でも定義が必要）
uint8_t u8x8_gpio_pico(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr) {
    return 1;
}
