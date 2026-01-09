#include "ball.hpp"
#include "display.hpp"
#include "rp2040.hpp"
#include "../config.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "picoPioUart.pio.h"
#include <math.h>

uint8_t buffer2[32];
uint16_t BallSensor[16];

bool DoneBallSensor[16];
float BallVector[8];
int BallWeight[8];
int BallTotalWeight;
float VectorX,VectorY;
int BallVectorNumber;

PIO pio_ball;
uint sm_rx_ball;
uint sm_tx_ball;
uint offset_ball;
uint offset2_ball;
bool parity_check_ball;

//ボールセンサーの初期化
void BallSetup(){
    pio_ball = pio0;

    sm_rx_ball = 2;

    //使うピン番号が32以上なので必要な処理群
    // pio_set_gpio_base(pio_ball,16);

    offset_ball = pio_add_program(pio_ball, &picoPioUartRx_program);
    picoPioUartRx_program_init(pio_ball, sm_rx_ball, offset_ball, (uint)(BallSCLpin), SERIAL_BAUD);

    // 使うSMを指定する(送信と受信では別のSMを使う)
    sm_tx_ball = 3;
    offset2_ball = pio_add_program(pio_ball, &picoPioUartTx_program);
    picoPioUartTx_program_init(pio_ball, sm_tx_ball, offset2_ball, (uint)(BallSDApin), SERIAL_BAUD);
}

//ボールセンサー(赤外線センサー)を使う。
void UseBallSensor(){
    //ボール検知センサを調べる
    picoPioUartTx_program_putc(0x01,true);
    for(int i = 0;i <= 31;i++){
      buffer2[i] = picoPioUartRx_program_getc(true,&parity_check_ball);
    }
    i2c_read_blocking(BallI2C,0x42,buffer2,32,false);
    //データを16bitのもとの形に直す
    for(int i = 0;i < 16;i++){
        BallSensor[i] = ((uint16_t)buffer2[i*2] << 8) | buffer2[i*2 + 1];
    }
    
    //データを使える形に変換する
    for(int i = 0;i < 16;i++){
      DoneBallSensor[i] = false;
      if(i < 8){
        BallVector[i] = 0;
        BallWeight[i] = 0;
      }
    }  
    BallVectorNumber = 0;
    for(int i = 0;i < 16;i++){
      if(BallSensor[i] > 0 && DoneBallSensor[i] == false){
        if(i == 0){
          //LineSensor[0]だけ反時計回り側にあるセンサを考える
          for(int j = 15;j >= 11;j--){
            if(BallSensor[j] > 0){
                DoneBallSensor[j] = true;
                if(j == 11){
                    BallVector[BallVectorNumber] -= (16 - j) * 11.25;
                    BallWeight[BallVectorNumber] += 16 - j;
                }
            }else{
                BallVector[BallVectorNumber] -= (15 - j) * 11.25;
                BallWeight[BallVectorNumber] += 15 - j;
                break;
            }
          }
        }
        for(int j = 1;j <= 5;j++){
            //存在しない値を考えないようにする
            if(i >= 16 - j){
                BallVector[BallVectorNumber] += (j - 1) * 11.25 + 22.5 * i;
                BallWeight[BallVectorNumber] += j;
                break;
            }
            if(BallSensor[i + j] > 0){
                DoneBallSensor[i + j] = true;
                if(j == 5){
                    BallVector[BallVectorNumber] += 56.25 + 22.5 * i;
                    BallWeight[BallVectorNumber] += 6;
                }
            }else{
                BallVector[BallVectorNumber] += (j - 1)*11.25 + 22.5 * i;
                BallWeight[BallVectorNumber] += j;
                break;
            }
        }
        BallVectorNumber++;
        DoneBallSensor[i] = true;
      }
    }
    //ベクトルの合成をする
    VectorX = 0;VectorY = 0;BallTotalWeight = 0;
    for(int i = 0;i < BallVectorNumber;i++){
      VectorX -= sin(BallVector[i] / 180.0 * 3.1415) * BallWeight[i];
      VectorY += cos(BallVector[i] / 180.0 * 3.1415) * BallWeight[i];
      BallTotalWeight += BallWeight[i];
      if(SerialWatch == "bav"){
        if(isUseDisplay){
          
        }else{
          printf("%d : %f\n",BallWeight[i],BallVector[i]);
        }
      }
    }
    if(SerialWatch == "bav" && isUseDisplay)WriteTextOnDisplay(5,30,"Not Found",8,false,true);
    if(BallVectorNumber == 0){
      VectorX = 999;
      VectorY = 999;
    }else{
      VectorX /= (float)BallTotalWeight;
      VectorY /= (float)BallTotalWeight;
    }

    if(VectorX == 999 && VectorY == 999){
      BallAngle = -999;
    }else if(VectorX == 0 && VectorY == 0){
      BallAngle = 0;
    }else{
      if(atan2(VectorY,VectorX) < 1.57){
        BallAngle = int((1.57 - atan2(VectorY,VectorX)) / 3.1415 * 16) * 11.25;
      }else{
        BallAngle = int((7.85 - atan2(VectorY,VectorX)) / 3.1415 * 16) * 11.25;
      }
    }
    //角度の微調整
    if(346 < BallAngle && BallAngle < 350){
      BallAngle = 0;
    }else if(BallAngle != 999){
      BallAngle += 11.25;
    }
    if(SerialWatch == "bal"){
      if(isUseDisplay){
        snprintf(DisplayBuffer,DisplayBufferSize,"%f",BallAngle);
        WriteTextOnDisplay(5,40,DisplayBuffer,12,false,true);
      }else{
        printf("%f\n",BallAngle);
      }
    }
}

/*
//UART(シリアル通信)で送信する関数
//
//data : 送るデータ(uint8_t型)
//even_parity : 偶数か奇数のどちらになるようにパリティを付加するか。trueで偶数。falseで奇数。
void picoPioUartTx_program_putc(unsigned char data, bool even_parity) {
    uint32_t byte = (uint32_t)data;
    uint8_t parity = 0;
    for (int i = 0; i < 8; i++) {
        parity ^= byte & 0x1;
        byte >>= 1;
    }
    byte = (uint32_t)data;
    if (parity) {
        if (even_parity) {
            byte |= 0x100;  // 偶数になるようにパリティを付加します
        }
    } else {
        if (!even_parity) {
            byte |= 0x100;  // 奇数になるようにパリティを付加します
        }
    }
    pio_sm_put_blocking(pio, sm_tx, (uint32_t)byte);  // TX FIFOへputします
}

//UART(シリアル通信)で受信する関数
//
//
//even_parity : 偶数か奇数のどちらになるようにパリティを付加されているか。trueで偶数。falseで奇数。
//parity_check : パリティビットの結果。正しいならtrue。違ったらfalseで、例外処理を用意する。データがなくてもfalseになる。
unsigned char picoPioUartRx_program_getc(bool even_parity,bool* parity_check) {
    // if(pio_sm_is_rx_fifo_empty(pio, sm_rx)){
        // *parity_check = false;
        // return 0;
    // }else{
     while (pio_sm_is_rx_fifo_empty(pio, sm_rx)) {
        tight_loop_contents();
        // printf("待機中");
     }
     
    uint32_t c32 = pio_sm_get(pio, sm_rx);

    c32 >>= 23;
    //パリティビットの検証をする
    bool real_parity = (c32 & 0x100) != 0;
    uint8_t byte = c32 & 0xff;

    uint8_t pcheck = 0;
    for (int i = 0; i < 8; i++) {
        pcheck ^= byte & 0x1;
        byte >>= 1;
    }

    *parity_check = (pcheck == real_parity);

    return (unsigned char)(c32 & 0xff);
    // }
}
    */