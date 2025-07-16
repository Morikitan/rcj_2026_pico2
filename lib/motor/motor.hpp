#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void MotorSetup();
void MainMotorState(int motor,int state,int speed);
void DribblerMotorState(int state,int speed);
void analogWrite(int gpio,int duty);

#ifdef __cplusplus
}
#endif