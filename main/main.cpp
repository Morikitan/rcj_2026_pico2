#include <stdio.h>
#include "pico/stdlib.h"
#include "display/display.hpp"
#include "rp2040/rp2040.hpp"
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
    //DisplaySetup(&u8g2);
    printf("初期化終了\n");
    RP2040Setup();
    while (true) {
        //UseDisplay(&u8g2);
        // UseDisplay(&u8g2);
        //UseEncoder();
        //picoPioUartRx_program_putc(0x65,true);
        
        bool parity_check;
        unsigned char data = picoPioUartRx_program_getc(true,&parity_check);
        if(parity_check == true){
            printf("結果 %u\n",data);
        }
    }
    
}
