#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void VariableSetup();
void PinSetup();
void Brake();
void UseAllSensor();
bool TurnToTargetAngle(float angle, bool isUseIsBreak);
float radian(float angle);
float radian45(float angle);
void MeasureMaxSpeed();

#ifdef __cplusplus
}
#endif