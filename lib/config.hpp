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
extern int MotorDuty[4];

//dribbler motor
#define DribblerMotorpin1 1
#define DribblerMotorpin2 1

//ball sensor
#define BallI2C i2c0
#define BallSDApin 16
#define BallSCLpin 17
extern float BallAngle;  //999.0でボール持ってる。-999.0で行方不明
extern int BallDistance;

//camera
#define CameraUART uart1
#define CameraTXpin 4
#define CameraRXpin 5
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
#define GyroSDApin 14
#define GyroSCLpin 15

extern float AngleX, AngleY, AngleZ;

//line sensor
#define LineUart uart0
#define LineTXpin 12
#define LineRXpin 13
extern char LineSensorA[5];
extern char LineSensorB[9];
extern char LineSensorC[9];
extern char LineSensorD[9];
extern char LineSensorE[48];
extern int AllLineSensorA, AllLineSensorB, AllLineSensorC, AllLineSensorD, AllLineSensorE, AllLineSensor;
extern int ErorrLineSensor;

//UART for RP2040
#define RP2040_UART_TXpin 43
#define RP2040_UART_RXpin 44
#define RP2040_UART_IRQpin 45
#define SERIAL_BAUD 125000

//encoder
extern bool isMotorClockWise[4];
extern float motorFrequency[4];

//others
