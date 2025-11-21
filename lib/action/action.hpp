#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void VariableSetup();
void PinSetup();
void Brake();
void UseMotorDuty();
void UseAllSensor();
bool TurnToTargetAngle(float angle, bool isUseIsBreak);

#ifdef __cplusplus
}
#endif