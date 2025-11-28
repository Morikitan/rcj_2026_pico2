#include "action/action.hpp"
#include "attack/attack.hpp"
#include "ball/ball.hpp"
#include "camera/camera.hpp"
#include "defence/defence.hpp"
#include "display/display.hpp"
#include "gyro/gyro.hpp"
#include "line/line.hpp"
#include "motor/motor.hpp"
#include "rp2040/rp2040.hpp"
#include "config.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "u8g2.h"
#include <stdio.h>

int main()
{
    stdio_init_all();

    sleep_ms(5000);
    
    printf("初期化開始\n");
    // ディスプレイ初期化（I2C + ノーブランドSSD1306用）
    //DisplaySetup(&u8g2);
    RP2040Setup();
    printf("初期化終了\n");
    
    gpio_init(RP2040_UART_TXpin);
    gpio_set_dir(RP2040_UART_TXpin,GPIO_IN);
    while (true) {
        //UseDisplay(&u8g2);
        // UseDisplay(&u8g2);
        //UseEncoder();
        //picoPioUartRx_program_putc(0x65,true);
        
        bool parity_check;
        unsigned char data = picoPioUartRx_program_getc(true,&parity_check);
        if(parity_check == true){
            printf("結果 %x\n",data);
        }
        printf("%d\n",(int)gpio_get(12));
        if(mode == 1 || mode == 2){
            Attack();
        }


    }
    
}
