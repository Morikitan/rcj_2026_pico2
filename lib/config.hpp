#pragma once

#include <string>
/*******************
ang 機体の角度(AngleX)
bal ボールセンサーの値(BallDistance,BallAngle)
lin ラインセンサーの値(0か1で受け取る)
mot モーターに加わる電圧(AngleXもセットで)
tim 1回の経過時間(ミリ秒)
ble BLEの接続状況
*******************/
extern std::string SerialWatch = "ang";

//main motor
#define MMpin1_1 1
#define MMpin1_2 1
#define MMpin2_1 1
#define MMpin2_2 1
#define MMpin3_1 1
#define MMpin3_2 1
#define MMpin4_1 1
#define MMpin4_2 1

//dribbler motor
#define DMpin1 1
#define DMpin2 1

//ball sensor
#define BallUART uart1
extern float BallAngle;  //999.0でボール持ってる。-999.0で行方不明
extern int BallDistance;

//camera
#define CameraUART uart0
extern int MyGoalX, MyGoalY;
extern int OpponentGoalX, OpponentGoalY;
extern double MyGoalAngle, MyGoalDistance;
extern double OpponentGoalAngle, OpponentGoalDistance;
extern int LeftWall, RightWall;

//gyro sensor
#define GyroI2C i2c1

extern float AngleX, AngleY, AngleZ;

//line sensor
#define LineI2C i2c0
extern char LineSensorABCD[16];
extern char LineSensorE[16];
extern int AllLineSensorA, AllLineSensorB, AllLineSensorC, AllLineSensorD, AllLineSensorE, AllLineSensor;
extern int ErorrLineSensor;

//others
