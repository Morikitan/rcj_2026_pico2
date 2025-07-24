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
    
    printf("初期化開始\n");
    DisplaySetup(&u8g2);
    printf("初期化終了\n");
    while (true) {
        UseDisplay(&u8g2);
    }
    
}
