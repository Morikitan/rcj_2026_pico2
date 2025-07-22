
#pragma once

#include "u8g2.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t u8x8_byte_pico_i2c(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_pico(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr) ;

#ifdef __cplusplus
}
#endif

