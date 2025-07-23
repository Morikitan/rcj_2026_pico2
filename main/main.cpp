#include <stdio.h>
#include "pico/stdlib.h"
#include "display/display.hpp"
#include "u8g2.h"

u8g2_t u8g2;

int main()
{
    stdio_init_all();

    DisplaySetup(&u8g2);

    while (true) {
        UseDisplay(&u8g2);
    }
}
