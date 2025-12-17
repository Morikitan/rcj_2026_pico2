#include "ball.hpp"
#include "display.hpp"
#include "rp2040.hpp"
#include "../config.hpp"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <math.h>

uint8_t buffer2[32];
uint16_t BallSensor[16];

bool DoneBallSensor[16];
float BallVector[8];
int BallWeight[8];
int BallTotalWeight;
float VectorX,VectorY;
int BallVectorNumber;

//ボールセンサーの初期化
void BallSetup(){
    gpio_init(BallSDApin);
    gpio_init(BallSCLpin);
    gpio_set_function(BallSDApin,GPIO_FUNC_I2C);
    gpio_set_function(BallSCLpin,GPIO_FUNC_I2C);
    i2c_init(BallI2C,115200);
}

//ボールセンサー(赤外線センサー)を使う。
void UseBallSensor(){
    //ボール検知センサを調べる
    i2c_write_blocking(BallI2C,0x42,(uint8_t[]){0x01},1,false);
    while(!i2c_get_read_available(BallI2C)){}
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
    if(SerialWatch == "bav" && isUseDisplay)WriteTextOnDisplay(5,20,"Not Found",8,false,true);
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
}