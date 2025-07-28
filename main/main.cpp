#include <stdio.h>
#include "pico/stdlib.h"
#include "display/display.hpp"
#include "u8g2.h"
#include "hardware/i2c.h"
#include "config.hpp"

u8g2_t u8g2;

int main()
{
    stdio_init_all();

    sleep_ms(5000);
    
    printf("初期化開始\n");
    // ディスプレイ初期化（I2C + ノーブランドSSD1306用）
    i2c_init(DisplayI2C, 400 * 1000);  // 400kHz
    gpio_set_function(DisplaySDApin, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(DisplaySCLpin, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(DisplaySDApin);
    gpio_pull_up(DisplaySCLpin);
    printf("1");
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, U8G2_R0, u8x8_byte_pico_i2c, u8x8_gpio_and_delay_cb);
    printf("2");
    u8g2_SetI2CAddress(&u8g2, 0x78); // I2Cアドレス (8bit形式)
    printf("3");
    u8g2_InitDisplay(&u8g2);
    printf("4");
    u8g2_SetPowerSave(&u8g2, 0); // 電源ON
    printf("初期化終了\n");
    while (true) {
        u8g2_SetContrast(&u8g2, 128);  // 最大
        u8g2_ClearBuffer(&u8g2);                  // バッファをクリア
        u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr); // フォント選択
        u8g2_DrawStr(&u8g2, 0, 24, "Hello Pico!"); // 文字列描画
        printf("Hello Pico!\n");
        u8g2_SendBuffer(&u8g2);                   // 表示に反映
        sleep_ms(1000);
        //UseDisplay(&u8g2);
    }
    
}
