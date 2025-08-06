#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "motor.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "../config.hpp"

//足回りモーターのピンのモード設定
void MotorSetup(){
    gpio_set_function(MainMotorPin1_1,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_2,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_1,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_2,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_1,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_2,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin1_1,GPIO_FUNC_PWM);
    gpio_set_function(MainMotorPin4_2,GPIO_FUNC_PWM);
}

//motor : 1～4
//state : 0 → 正転, 1 → 逆転, 2 → 静止, 3 → ブレーキ
//speed : 0～255
void MainMotorState(int motor,int state,int speed){
    if (motor == 1) {
        if (state == 0) {
            analogWrite(MainMotorPin1_1, speed);
            analogWrite(MainMotorPin1_2, 0);
        } else if (state == 1) {
            analogWrite(MainMotorPin1_1, 0);
            analogWrite(MainMotorPin1_2, speed);
        } else if (state == 2) {
            analogWrite(MainMotorPin1_1, 0);
            analogWrite(MainMotorPin1_2, 0);
        } else if (state == 3) {
            analogWrite(MainMotorPin1_1, speed);
            analogWrite(MainMotorPin1_2, speed);
        }
    } else if (motor == 2) {
        if (state == 0) {
            analogWrite(MainMotorPin2_1, speed);
            analogWrite(MainMotorPin2_2, 0);
        } else if (state == 1) {
            analogWrite(MainMotorPin2_1, 0);
            analogWrite(MainMotorPin2_2, speed);
        } else if (state == 2) {
            analogWrite(MainMotorPin2_1, 0);
            analogWrite(MainMotorPin2_2, 0);
        } else if (state == 3) {
            analogWrite(MainMotorPin2_1, speed);
            analogWrite(MainMotorPin2_2, speed);
        }
    } else if (motor == 3) {
        if (state == 0) {
            analogWrite(MainMotorPin3_1, speed);
            analogWrite(MainMotorPin3_2, 0);
        } else if (state == 1) {
            analogWrite(MainMotorPin3_1, 0);
            analogWrite(MainMotorPin3_2, speed);
        } else if (state == 2) {
            analogWrite(MainMotorPin3_1, 0);
            analogWrite(MainMotorPin3_2, 0);
        } else if (state == 3) {
            analogWrite(MainMotorPin3_1, speed);
            analogWrite(MainMotorPin3_2, speed);
        }
    } else if (motor == 4) {
        if (state == 0) {
            analogWrite(MainMotorPin4_1, speed);
            analogWrite(MainMotorPin4_2, 0);
        } else if (state == 1) {
            analogWrite(MainMotorPin4_1, 0);
            analogWrite(MainMotorPin4_2, speed);
        } else if (state == 2) {
            analogWrite(MainMotorPin4_1, 0);
            analogWrite(MainMotorPin4_2, 0);
        } else if (state == 3) {
            analogWrite(MainMotorPin4_1, speed);
            analogWrite(MainMotorPin4_2, speed);
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
