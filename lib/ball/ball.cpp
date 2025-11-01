#include "ball.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "rp2040.hpp"
#include "math.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

uint8_t buffer[32];
uint16_t BallSensor[16];

bool DoneBallSensor[16];
float Vector[8];
int Weight[8];
int TotalWeight;
float VectorX,VectorY;
int VectorNumber;

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
    i2c_read_blocking(BallI2C,0x42,buffer,32,false);
    //データを16bitのもとの形に直す
    for(int i = 0;i < 16;i++){
        BallSensor[i] = ((uint16_t)buffer[i*2] << 8) | buffer[i*2 + 1];
    }
    
    //データを使える形に変換する
    for(int i = 0;i < 16;i++){
      DoneBallSensor[i] = false;
      if(i < 8){
        Vector[i] = 0;
        Weight[i] = 0;
      }
    }  
    VectorNumber = 0;
    for(int i = 0;i < 16;i++){
      if(BallSensor[i] > 0 && DoneBallSensor[i] == false){
        if(i == 0){
          //LineSensor[0]だけ反時計回り側にあるセンサを考える
          for(int j = 15;j >= 11;j--){
            if(BallSensor[j] > 0){
                DoneBallSensor[j] = true;
                if(j == 11){
                    Vector[VectorNumber] -= (16 - j) * 11.25;
                    Weight[VectorNumber] += 16 - j;
                }
            }else{
                Vector[VectorNumber] -= (15 - j) * 11.25;
                Weight[VectorNumber] += 15 - j;
                break;
            }
          }
        }
        for(int j = 1;j <= 5;j++){
            //存在しない値を考えないようにする
            if(i >= 16 - j){
                Vector[VectorNumber] += (j - 1) * 11.25 + 22.5 * i;
                Weight[VectorNumber] += j;
                VectorNumber++;
                break;
            }
            if(BallSensor[i + j] > 0){
                DoneBallSensor[i + j] = true;
                if(j == 5){
                    Vector[VectorNumber] += 56.25 + 22.5 * i;
                    Weight[VectorNumber] += 6;
                }
            }else{
                Vector[VectorNumber] += (j - 1)*11.25 + 22.5 * i;
                Weight[VectorNumber] += j;
                break;
            }
        }
        VectorNumber++;
        DoneBallSensor[i] = true;
      }
    }
    //ベクトルの合成をする
    if(SerialWatch == "bal")printf(" ベク ");
    VectorX = 0;VectorY = 0;TotalWeight = 0;
    for(int i = 0;i < VectorNumber;i++){
      VectorX -= sin(Vector[i] / 180.0 * 3.1415) * Weight[i];
      VectorY += cos(Vector[i] / 180.0 * 3.1415) * Weight[i];
      if(SerialWatch == "bal"){
        printf("%d : %f\n",Weight[i],Vector[i]);
        TotalWeight += Weight[i];
      }
    }
    if(VectorNumber == 0){
      VectorX = 999;
      VectorY = 999;
    }else{
      VectorX /= (float)TotalWeight;
      VectorY /= (float)TotalWeight;
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
    if(BallAngle == 348.75){
      BallAngle = 0;
    }else if(BallAngle != 999){
      BallAngle += 11.25;
    }
}