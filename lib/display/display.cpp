
// u8g2_user_impl.cpp
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "display.hpp"
#include "../config.hpp"
#include "u8g2.h"

void DisplaySetup(u8g2_t *u8g2){
    // ディスプレイ初期化（I2C + ノーブランドSSD1306用）
    i2c_init(DisplayI2C, 400 * 1000);  // 400kHz
    gpio_set_function(DisplaySDApin, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(DisplaySCLpin, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(DisplaySDApin);
    gpio_pull_up(DisplaySCLpin);
    printf("1");
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        u8g2, U8G2_R0, u8x8_byte_pico_i2c, u8x8_gpio_and_delay_cb);
    printf("2");
    u8g2_SetI2CAddress(u8g2, 0x78); // I2Cアドレス (8bit形式)
    printf("3");
    u8g2_InitDisplay(u8g2);
    printf("4");
    u8g2_SetPowerSave(u8g2, 0); // 電源ON
}

void UseDisplay(u8g2_t *u8g2){
    u8g2_SetContrast(u8g2, 128);  // 最大
    u8g2_ClearBuffer(u8g2);                  // バッファをクリア
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr); // フォント選択
    u8g2_DrawStr(u8g2, 0, 24, "Hello World!"); // 文字列描画
    printf("I hate you!\n");
    u8g2_SendBuffer(u8g2);                   // 表示に反映
    sleep_ms(1000);
}

// I2C: GP4 = SDA, GP5 = SCL, I2C0使用
uint8_t u8x8_byte_pico_i2c(u8x8_t *u8x8, uint8_t msg,uint8_t arg_int, void *arg_ptr) {
    static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;
 
    switch(msg)
    {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;      
            while( arg_int > 0 )
            {
	            buffer[buf_idx++] = *data;
	            data++;
	            arg_int--;
            }      
            break;
        case U8X8_MSG_BYTE_INIT:
            /* add your custom code to init i2c subsystem */
            break;
        case U8X8_MSG_BYTE_SET_DC:
            /* ignored for i2c */
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            buf_idx = 0;
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            i2c_write_blocking(i2c1,u8x8_GetI2CAddress(u8x8) >> 1,buffer,buf_idx,false);
            break;
        default:
            return 0;
  }
  return 1;
}

// GPIOコールバック（未使用でも定義が必要）
uint8_t u8x8_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg) {
        case U8X8_MSG_DELAY_MILLI:
            sleep_ms(arg_int);
            break;
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
        case U8X8_MSG_GPIO_RESET:
        case U8X8_MSG_GPIO_CS:
        case U8X8_MSG_GPIO_DC:
        case U8X8_MSG_GPIO_I2C_CLOCK:
        case U8X8_MSG_GPIO_I2C_DATA:
            // I2CはGPIO操作不要
            break;
    }
    return 1;
}