#include "ball.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

uint8_t buffer[32];
uint16_t pulse[16];

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
    i2c_write_blocking(BallI2C,0x42,(uint8_t[]){0x01},1,false);
    while(!i2c_get_read_available(BallI2C)){}
    i2c_read_blocking(BallI2C,0x42,buffer,32,false);
    //データを16bitのもとの形に直す
    for(int i = 0;i < 16;i++){
        pulse[i] = buffer[i*2] << 8 & 0xFFFF;
        pulse[i] |= buffer[i * 2 + 1];
    }
}