#include <stdio.h>
#include "pico/stdlib.h"
#include "display/display.hpp"
#include "u8g2.h"
#include "hardware/i2c.h"

u8g2_t u8g2;

int main()
{
    stdio_init_all();

    sleep_ms(5000);
    /*
    printf("初期化開始\n");
    DisplaySetup(&u8g2);
    printf("初期化終了\n");
    while (true) {
        UseDisplay(&u8g2);
    }
    */
   /*
    i2c_init(i2c1, 100 * 1000);
    gpio_set_function(31, GPIO_FUNC_I2C);
    gpio_set_function(32, GPIO_FUNC_I2C);
    //gpio_pull_up(31);
    //gpio_pull_up(32);

    uint8_t address = 0x3C;
    uint8_t dummy = 0;
    while(true){
    int ret = i2c_write_blocking(i2c0, address, &dummy, 0, false);
    if (ret < 0) {
        printf("I2C device not found at 0x3C\n");
    } else {
        printf("I2C device found at 0x3C\n");
    }
    }
    */
}
