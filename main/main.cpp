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

    sleep_ms(1000);
    printf("初期化開始\n");
    PinSetup();
    // ディスプレイ初期化（I2C + ノーブランドSSD1306用）
    DisplaySetup();
    RP2040Setup();
    printf("初期化終了\n");
    while (true) {
        //UseEncoder();
        // picoPioUartRx_program_putc(0x65,true);
        //picoPioUartTx_program_putc(0x68,true);
        //printf("送信いたしました");
        //sleep_ms(1000);
        if(isUseDisplay) PrintDisplayMode();
        //WriteTextOnDisplay(5,15,"<Home>",12,true,true);
        sleep_ms(1000);

        if(mode == 1 || mode == 2){
            Attack();
        }else if(mode == 3 || mode == 4){
            Defence();
        }
    }
}
