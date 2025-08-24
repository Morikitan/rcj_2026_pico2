#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Attack();
void NewLineMove();
void LineMove();
void ChaseBall(float angle,bool isMakao);
void Makao(bool isClockwise,int TargetAngle);
void NonDribbler(float angle,bool isClockWise);

#ifdef __cplusplus
}
#endif