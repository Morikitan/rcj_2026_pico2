#pragma once

#include <string>
/*******************
ang 機体の角度(AngleX)
bal ボールセンサーの値(BallDistance,BallAngle)
cam カメラの値(自分と敵のゴールの距離と向き、壁との距離)
lin ラインセンサーの値(0か1で受け取る)
mot モーターに加わる電圧(AngleXもセットで)
tim 1回の経過時間(ミリ秒)
*******************/
extern std::string SerialWatch;

//mode
extern int mode;  //mode = 99で全部正転
extern char isYellowMyGoal;

//speed
#define LeastTurnSpeed 100
#define LeastSpeed 120
#define TurnSpeed 120
#define DefaultSpeed 220
#define DefaultSpeed1 170
#define DefaultSpeed2 170
#define DefaultSpeed3 170
#define DefaultSpeed4 170
#define LargeDefaultSpeed1 200
#define LargeDefaultSpeed2 200
#define LargeDefaultSpeed3 200
#define LargeDefaultSpeed4 200
#define LineSpeed 220
#define DefenceSpeed 120
#define SinSpeed 20
#define VectorSpeed 30

//main motor
#define MainMotorPin1_1 1
#define MainMotorPin1_2 1
#define MainMotorPin2_1 1
#define MainMotorPin2_2 1
#define MainMotorPin3_1 1
#define MainMotorPin3_2 1
#define MainMotorPin4_1 1
#define MainMotorPin4_2 1
int MotorDuty[4];

//dribbler motor
#define DribblerMotorpin1 1
#define DribblerMotorpin2 1

//ball sensor
#define BallUART uart1
extern float BallAngle;  //999.0でボール持ってる。-999.0で行方不明
extern int BallDistance;

//camera
#define CameraUART uart0
#define CameraTXpin 1
#define CameraRXpin 2
struct GoalInformation{
    int X;
    int Y;
    float angle;
    float distance;
};
extern struct GoalInformation MyGoal;
extern struct GoalInformation OpponentGoal;
extern int LeftWall, RightWall;

//display
#define DisplayI2C i2c1
#define DisplaySDApin 2
#define DisplaySCLpin 3

//gyro sensor
#define GyroI2C i2c1
#define GyroSDApin 2
#define GyroSCLpin 2

extern float AngleX, AngleY, AngleZ;

//line sensor
#define LineI2C i2c0
extern char LineSensorABCD[16];
extern char LineSensorE[16];
extern int AllLineSensorA, AllLineSensorB, AllLineSensorC, AllLineSensorD, AllLineSensorE, AllLineSensor;
extern int ErorrLineSensor;

//SPI
#define SPI_CSpin 9
#define SPI_SCKpin 10
#define SPI_RXpin 12
#define SPI_TXpin 11

//others
