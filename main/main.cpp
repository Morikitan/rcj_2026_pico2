#include <stdio.h>
#include "pico/stdlib.h"
#include "display/display.hpp"
#include "u8g2.h"

u8g2_t u8g2;

int main()
{
    stdio_init_all();
    stdio_init_all();

    // ディスプレイ初期化（I2C + ノーブランドSSD1306用）
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &u8g2, U8G2_R0, u8x8_byte_pico_i2c, u8x8_gpio_pico);
    u8g2_SetI2CAddress(&u8g2, 0x3C << 1); // I2Cアドレス (8bit形式)

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0); // 電源ON

    while (true) {
        u8g2_ClearBuffer(&u8g2);                  // バッファをクリア
        u8g2_SetFont(&u8g2, u8g2_font_ncenB08_tr); // フォント選択
        u8g2_DrawStr(&u8g2, 0, 24, "Hello Pico!"); // 文字列描画
        u8g2_SendBuffer(&u8g2);                   // 表示に反映
        sleep_ms(1000);
    }
}
