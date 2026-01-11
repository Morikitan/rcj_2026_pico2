#include "display.hpp"
#include "motor.hpp"
#include "rp2040.hpp"
#include "../config.hpp"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float output[4] = {0,0,0,0};
float preMotorFrequency[4] = {0,0,0,0};
double preTime[4];
double pretime = 0;

//足回りモーターのピンのモード設定
void MotorSetup(){
    gpio_init(PWMpin1);
    gpio_init(PWMpin2);
    gpio_init(PWMpin3);
    gpio_init(PWMpin4);
    gpio_init(MainMotorPin1_1);
    gpio_init(MainMotorPin1_2);
    gpio_init(MainMotorPin2_1);
    gpio_init(MainMotorPin2_2);
    gpio_init(MainMotorPin3_1);
    gpio_init(MainMotorPin3_2);
    gpio_init(MainMotorPin4_1);
    gpio_init(MainMotorPin4_2);
    gpio_set_function(PWMpin1,GPIO_FUNC_PWM);
    gpio_set_function(PWMpin2,GPIO_FUNC_PWM);
    gpio_set_function(PWMpin3,GPIO_FUNC_PWM);
    gpio_set_function(PWMpin4,GPIO_FUNC_PWM);
    gpio_set_dir(MainMotorPin1_1,GPIO_OUT);
    gpio_set_dir(MainMotorPin1_2,GPIO_OUT);
    gpio_set_dir(MainMotorPin2_1,GPIO_OUT);
    gpio_set_dir(MainMotorPin2_2,GPIO_OUT);
    gpio_set_dir(MainMotorPin3_1,GPIO_OUT);
    gpio_set_dir(MainMotorPin3_2,GPIO_OUT);
    gpio_set_dir(MainMotorPin4_1,GPIO_OUT);
    gpio_set_dir(MainMotorPin4_2,GPIO_OUT);
}

//motor : 1～4
//state : 0 → 正転, 1 → 逆転, 2 → 静止, 3 → ブレーキ
//speed : 0～255
void MainMotorState(int motor,int state,int speed){
    if (motor == 1) {
        analogWrite(PWMpin1, speed);
        if (state == 0) {
            gpio_put(MainMotorPin1_1,1);
            gpio_put(MainMotorPin1_2,0);
        } else if (state == 1) {
            gpio_put(MainMotorPin1_1,0);
            gpio_put(MainMotorPin1_2,1);
        } else if (state == 2) {
            gpio_put(MainMotorPin1_1,0);
            gpio_put(MainMotorPin1_2,0);
        } else if (state == 3) {
            gpio_put(MainMotorPin1_1,1);
            gpio_put(MainMotorPin1_2,1);
        }
    } else if (motor == 2) {
        analogWrite(PWMpin2, speed);
        if (state == 0) {
            gpio_put(MainMotorPin2_1,1);
            gpio_put(MainMotorPin2_2,0);
        } else if (state == 1) {
            gpio_put(MainMotorPin2_1,0);
            gpio_put(MainMotorPin2_2,1);
        } else if (state == 2) {
            gpio_put(MainMotorPin2_1,0);
            gpio_put(MainMotorPin2_2,0);
        } else if (state == 3) {
            gpio_put(MainMotorPin2_1,1);
            gpio_put(MainMotorPin2_2,1);
        }
    } else if (motor == 3) {
        analogWrite(PWMpin1, speed);
        if (state == 0) {
            gpio_put(MainMotorPin3_1,1);
            gpio_put(MainMotorPin3_2,0);
        } else if (state == 1) {
            gpio_put(MainMotorPin3_1,0);
            gpio_put(MainMotorPin3_2,1);
        } else if (state == 2) {
            gpio_put(MainMotorPin3_1,0);
            gpio_put(MainMotorPin3_2,0);
        } else if (state == 3) {
            gpio_put(MainMotorPin3_1,1);
            gpio_put(MainMotorPin3_2,1);
        }
    } else if (motor == 4) {
        analogWrite(PWMpin1, speed);
        if (state == 0) {
            gpio_put(MainMotorPin4_1,1);
            gpio_put(MainMotorPin4_2,0);
        } else if (state == 1) {
            gpio_put(MainMotorPin4_1,0);
            gpio_put(MainMotorPin4_2,1);
        } else if (state == 2) {
            gpio_put(MainMotorPin4_1,0);
            gpio_put(MainMotorPin4_2,0);
        } else if (state == 3) {
            gpio_put(MainMotorPin4_1,1);
            gpio_put(MainMotorPin4_2,1);
        }
    }
}

//gpio : ピン番号
//duty : 0～255
void analogWrite(int gpio,int duty){
    //周波数をf[Hz]とすると
    //(pico2)150×1000×1000 = f × clkdiv × (warp + 1) clkdiv = 588.235
    //(pico) 125×1000×1000 = f × clkdiv × (warp + 1) clkdiv = 488.281
    //よって今は f = 1.0[kHz]
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint channel = pwm_gpio_to_channel(gpio);
    pwm_set_clkdiv(slice_num, 588.235);
    pwm_set_wrap(slice_num, 255);
    pwm_set_chan_level(slice_num, channel, duty);
    pwm_set_enabled(slice_num, true);
}

#define Kp 9.0
#define Ki 3.0
#define Kd 5.0

float I[4] = {0,0,0,0};
float preP[4] = {0,0,0,0};
//エンコーダーの値を取得して、各モーターへの出力を設定する関数
//タイヤの円周は6π≒19[cm] (参考)19/√2≒13.43
//float speed[4] : 1秒当たりの回転数(負の値も〇)
void EncoderAllMainMotorState(float speed[4]){
    /*UseEncoder();
    float now = time_us_64() / 1000000.0;
    for (int i = 0;i < 4;i++){
        float dt = now - preTime[i];
        float P = speed[i] - motorFrequency[i];
        if(output[i] != 255 && output[i] != -255) I[i] += P * dt;
        
        output[i] = P * Kp + I[i] * Ki + (P - preP[i]) * Kd + speed[i] * Kff;
        preP[i] = P;

        if(motorFrequency[i] < 0.1 && motorFrequency[i] > -0.1){
            if(speed[i] > 0){
                output[i] += LeastVoltage;
            }else if(speed[i] < 0){
                output[i] -= LeastVoltage;
            }
        }
        //-255～255の範囲にする
        if(output[i] > 255) {
            output[i] = 255.0;
        }else if(output[i] < -255){
            output[i] = -255.0;
        }
        if (fabs(output[i]) < 1.0) output[i] = 0; 

        preMotorFrequency[i] = motorFrequency[i];

        if(output[i] >= 0)MainMotorState(i+1, 0, (int)output[i]);
        else              MainMotorState(i+1, 1, (int)(output[i] * -1.0)); 

        preTime[i] = now;
    }
    if (SerialWatch == "mot") {
    if(isUseDisplay){
      snprintf(DisplayBuffer,DisplayBufferSize,"BallAngle : %.1f AngleX : %.1f",BallAngle,AngleX);
      WriteTextOnDisplay(5,15,DisplayBuffer,8,true,false);
      snprintf(DisplayBuffer,DisplayBufferSize,"motor1 : %.1f m2 : %.1f",TargetFrequency[0],TargetFrequency[1]);
      WriteTextOnDisplay(5,25,DisplayBuffer,8,false,false);
      snprintf(DisplayBuffer,DisplayBufferSize,"m3 : %.1f m4 : %.1f",TargetFrequency[2],TargetFrequency[3]);
      WriteTextOnDisplay(5,35,DisplayBuffer,8,false,false);
      snprintf(DisplayBuffer,DisplayBufferSize,"回転 : %.1f 縦 : %.1f 横 : %.1f",TargetFrequency[0] + TargetFrequency[1] - TargetFrequency[2] - TargetFrequency[3],TargetFrequency[0] + TargetFrequency[1] + TargetFrequency[2] + TargetFrequency[3],TargetFrequency[0] - TargetFrequency[1] + TargetFrequency[2] - TargetFrequency[3]);
      WriteTextOnDisplay(5,45,DisplayBuffer,8,false,true);
    }else{
      printf("BallAngle : %f AngleX : %f",BallAngle,AngleX);
      printf(" motor1 : %f m2 : %f m3 : %f m4 : %f",TargetFrequency[0],TargetFrequency[1],TargetFrequency[2],TargetFrequency[3]);
      printf(" 回転 : %d 縦 : %d 横 : %d\n",TargetFrequency[0] + TargetFrequency[1] - TargetFrequency[2] - TargetFrequency[3],TargetFrequency[0] + TargetFrequency[1] + TargetFrequency[2] + TargetFrequency[3],TargetFrequency[0] - TargetFrequency[1] + TargetFrequency[2] - TargetFrequency[3]);  //反時計が正
    }
  }
    //pretime = time_us_64() / 1000000.0;
    */
   for(int i = 0;i < 4;i++){
        if(speed[i] > 0){
            MainMotorState(i+1,0,(int)(speed[i] / MaxFrequency * 255));
        }else if(speed[i] < 0){
            MainMotorState(i+1,1,(int)(speed[i] / MaxFrequency * -255));
        }else if(speed[i] == 0){
            MainMotorState(i+1,2,0);
        }
   }
}


//※使う前に必ずUseEncoderを実行する必要がある
//既存のエンコーダーのデータからモーターへの出力を設定する関数
//motor : 1～4
//speed : 1秒当たりの回転数(負の値も〇)
void EncoderMainMotorState(int motor, float speed){
    /*output[motor - 1] += (speed - motorFrequency[motor - 1]) * Kp;
    //-255～255の範囲にする
    if(output[motor - 1] > 255) output[motor - 1] = 255.0;
    else if(output[motor - 1] < -255) output[motor - 1] = -255.0;

    // preMotorFrequency[i] = motorFrequency[i];

    if(output[motor - 1] >= 0)MainMotorState(motor, 0, (int)output[motor - 1]);
    else              MainMotorState(motor, 1, (int)(output[motor - 1] * -1.0)); */
    if(speed > 0){
        MainMotorState(motor,0,(int)(speed / MaxFrequency * 255));
    }else if(speed < 0){
        MainMotorState(motor,1,(int)(speed / MaxFrequency * -255));
    }else if(speed == 0){
        MainMotorState(motor,2,0);
    }
}

