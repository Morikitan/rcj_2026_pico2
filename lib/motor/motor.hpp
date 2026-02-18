#pragma once

#include "pico/stdlib.h"

#ifdef __cplusplus
extern "C" {
#endif

void MotorSetup();
void MainMotorState(int motor,int state,int speed);
void analogWrite(int gpio,int duty);
void EncoderAllMainMotorState(float speed[4]);
void EncoderMainMotorState(int motor, float speed);

#ifdef __cplusplus
}
#endif