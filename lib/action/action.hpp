#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void VariableSetup();
void PinSetup();
void Brake();
void UseMotorDuty();
void UseAllSensor();
void TurnToTargetAngle(float angle);

#ifdef __cplusplus
}
#endif