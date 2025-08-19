#include "action.hpp"
#include "motor.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

//変数の初期化
void VariableSetup(){

}

//その他のピンの初期化
void PinSetup(){

}

//足回りモーターすべてに、最大の強さのブレーキをかける。
void Brake(){
    MainMotorState(1,3,255);
    MainMotorState(2,3,255);
    MainMotorState(3,3,255);
    MainMotorState(4,3,255);
}

//MotorDuty配列の値をMainMotorStateで出力する。
void UseMotorDuty(){
    for (int a = 0; a <= 3; a++) {
        if (MotorDuty[a] > 255) {
            MainMotorState(a + 1, 0, 255);
        } else if (MotorDuty[a] > 0) {
            MainMotorState(a + 1, 0, (int)(MotorDuty[a]));
        } else if (MotorDuty[a] > -255) {
            MainMotorState(a + 1, 1, (int)(MotorDuty[a] * -1));
        } else {
            MainMotorState(a + 1, 1, 255);
        }
    }
}

//すべてのセンサーを使用する。
void UseAllSensor(){

}