#include "action.hpp"
#include "display.hpp"
#include "line.hpp"
#include "../config.hpp"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <math.h>
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

    if(SerialWatch == "lin" && !isUseDisplay) printf("A:");
    for(int i = 0;i < 5;i++){
        LineSensorA[i] = (char)((LineRawData[0] >> (7 - i)) & (0x01));
        AllLineSensorA += LineSensorA[i];
        if(SerialWatch == "lin"){
            if(isUseDisplay){
                if(LineSensorA[i] == 1){
                    DrawPixelOnDisplay(102-3*i,8);
                }
            }else{
                printf("%d",LineSensorA[i]);
            }
        }
    }

    if(SerialWatch == "lin" && !isUseDisplay) printf(" B:");
    for(int i = 0;i < 8;i++){
        LineSensorB[i] = (char)((LineRawData[1] >> (7 - i)) & (0x01));
        AllLineSensorB += LineSensorB[i];
        if(SerialWatch == "lin"){
            if(isUseDisplay){
                if(LineSensorB[i] == 1){
                    if(i == 0) DrawPixelOnDisplay(69,26);
                    else if(i == 1) DrawPixelOnDisplay(67,29);
                    else if(i == 2) DrawPixelOnDisplay(66,32);
                    else if(i == 3) DrawPixelOnDisplay(67,35);
                    else if(i == 4) DrawPixelOnDisplay(69,38);
                    else DrawPixelOnDisplay(58 + 2*i,32);
                }
            }else{
                printf("%d",LineSensorB[i]);
            }
        }
    }
    LineSensorB[8] = LineRawData[4] >> 7;
    AllLineSensorB += LineSensorB[8];
    if(SerialWatch == "lin"){
        if(isUseDisplay){
            if(LineSensorB[8] == 1){
                DrawPixelOnDisplay(74,32);
            }
        }else{
            printf("%d",LineSensorB[8]);
        }
    }

    if(SerialWatch == "lin" && !isUseDisplay) printf(" C:");
    for(int i = 0;i < 8;i++){
        LineSensorC[i] = (char)((LineRawData[2] >> (7 - i)) & (0x01));
        AllLineSensorC += LineSensorC[i];
        if(SerialWatch == "lin"){
            if(isUseDisplay){
                if(LineSensorC[i] == 1){
                    if(i == 0) DrawPixelOnDisplay(90,58);
                    else if(i == 1) DrawPixelOnDisplay(93,60);
                    else if(i == 2) DrawPixelOnDisplay(96,61);
                    else if(i == 3) DrawPixelOnDisplay(99,60);
                    else if(i == 4) DrawPixelOnDisplay(102,58);
                    else DrawPixelOnDisplay(96,69 - 2*i);
                }
            }else{
                printf("%d",LineSensorC[i]);
            }
        }
    }
    LineSensorC[8] = LineRawData[4] >> 7;
    AllLineSensorC += LineSensorC[8];
    if(SerialWatch == "lin"){
        if(isUseDisplay){
            if(LineSensorC[8] == 1){
                DrawPixelOnDisplay(96,53);
            }
        }else{
            printf("%d",LineSensorC[8]);
        }
    }

    if(SerialWatch == "lin" && !isUseDisplay) printf(" D:");
    for(int i = 0;i < 8;i++){
        LineSensorD[i] = (char)((LineRawData[3] >> (7 - i)) & (0x01));
        AllLineSensorD += LineSensorD[i];
        if(SerialWatch == "lin"){
            if(isUseDisplay){
                if(LineSensorD[i] == 1){
                    if(i == 0) DrawPixelOnDisplay(123,38);
                    else if(i == 1) DrawPixelOnDisplay(125,35);
                    else if(i == 2) DrawPixelOnDisplay(126,32);
                    else if(i == 3) DrawPixelOnDisplay(125,29);
                    else if(i == 4) DrawPixelOnDisplay(123,26);
                    else DrawPixelOnDisplay(134 - 2*i,32);
                }
            }else{
                printf("%d",LineSensorD[i]);
            }
        }
    }
    LineSensorD[8] = LineRawData[4] >> 7;
    AllLineSensorD += LineSensorD[8];
    if(SerialWatch == "lin"){
        if(isUseDisplay){
            if(LineSensorD[8] == 1){
                DrawPixelOnDisplay(118,32);
            }
        }else{
            printf("%d",LineSensorD[8]);
        }
    }

    if(SerialWatch == "lin" && !isUseDisplay) printf("\nE:");
    for(int i = 0;i < 48;i++){
        LineSensorE[i] = (char)((LineRawData[5 + i / 8] >> (7 - (i%8)) & (0x01)));
        AllLineSensorE += LineSensorE[i];
        if(SerialWatch == "lin"){
            if(isUseDisplay){
                if(LineSensorE[i] == 1){
                    DrawPixelOnDisplay(96+int(20*cos(radian(-3.75-7.5*i))),31+int(20*sin(radian(-3.75-7.5*i))));
                }
            }else{
                printf("%d",LineSensorE[i]);
            }
        }
    }

    if(SerialWatch == "lin"){
        if(isUseDisplay){
            //画面上に描画する処理を入れる
            //文字サイズを見てから考える
        }else{
            printf("\n");
        }
    }
}