#include "display.hpp"
#include "line.hpp"
#include "../config.hpp"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdio.h>

uint8_t data;
uint8_t LineRawData[11];

void LineSensorSetup(){
    gpio_init(LineTXpin);
    gpio_init(LineRXpin);
    gpio_set_function(LineTXpin,GPIO_FUNC_UART);
    gpio_set_function(LineRXpin,GPIO_FUNC_UART);
    uart_init(LineUart,115200);
}

void UseLineSensor(){
    //RXバッファを空にする
    while(uart_is_readable(LineUart)) uart_read_blocking(LineUart,&data,1);
    //atmegaにデータを要求
    uart_write_blocking(LineUart,(uint8_t[]){0x01},1);
    //データをすべて読み取る
    uart_read_blocking(LineUart,LineRawData,11);
    //データを変数に格納する
    AllLineSensorA = 0;AllLineSensorB = 0;AllLineSensorC = 0;AllLineSensorD = 0;AllLineSensorE = 0;

    for(int i = 0;i < 5;i++){
        LineSensorA[i] = (char)((LineRawData[0] >> (7 - i)) & (0x01));
        AllLineSensorA += LineSensorA[i];
    }

    for(int i = 0;i < 8;i++){
        LineSensorB[i] = (char)((LineRawData[1] >> (7 - i)) & (0x01));
        AllLineSensorB += LineSensorB[i];
    }
    LineSensorB[8] = LineRawData[4] >> 7;
    AllLineSensorB += LineSensorB[8];

    for(int i = 0;i < 8;i++){
        LineSensorC[i] = (char)((LineRawData[2] >> (7 - i)) & (0x01));
        AllLineSensorC += LineSensorC[i];
    }
    LineSensorC[8] = LineRawData[4] >> 7;
    AllLineSensorC += LineSensorC[8];

    for(int i = 0;i < 8;i++){
        LineSensorD[i] = (char)((LineRawData[3] >> (7 - i)) & (0x01));
        AllLineSensorD += LineSensorD[i];
    }
    LineSensorD[8] = LineRawData[4] >> 7;
    AllLineSensorD += LineSensorD[8];

    for(int i = 0;i < 48;i++){
        LineSensorE[i] = (char)((LineRawData[5 + i / 8] >> (7 - (i%8)) & (0x01)));
        AllLineSensorE += LineSensorE[i];
    }
}