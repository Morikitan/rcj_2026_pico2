#pragma once

#include <string>
/******************
    1 : hom 通常モード(modeを表示)
    2 : bal ボールセンサーの値(BallDistance,BallAngle)
    3 : bav ボールセンサのベクトルの値(消すかも)
    4 : cam カメラの値(自分と敵のゴールの距離と向き、壁との距離)
    5 : enc エンコーダーの値
    6 : gyr 機体の角度(AngleX)
    7 : lin ラインセンサーの値(0か1で受け取る)
    8 : mot モーターに加わる電圧(AngleXもセットで)
    9 : tim 1回の経過時間(ミリ秒)
   10 : vec ラインのベクトルを取得
   11 : oth その他(時によって変わる)
    *******************/
extern std::string SerialWatch;

//mode
extern int mode;  //mode = 99で全部正転
extern char isYellowMyGoal;

//speed
#define LeastVoltage 50
#define DefaultFrequency MaxFrequency / 2.0
#define MaxFrequency 8.5
#define MaxTurnFrequency 4.0
#define TurnFrequency 6.0
#define MaxAddTurnFrequency 1.0
#define AddTurnFrequency 2.0
#define MakaoFrequency 6.0
#define LineFrequency MaxFrequency
#define DefenceFrequency 4.0
#define SinFrequency 0.5
#define VectorFrequency 30

//for attack
#define GensokuDistance 75
#define Kff 30.0 //モーターのPID制御時に加えるff項の目標回転数にかける定数

//for defence
#define GoalDistance 75 //ゴールの線上にいるときの距離。ゴールの内側にいるか外側にいるかの区別をする用

//main motor
#define MainMotorPin1_1 30
#define MainMotorPin1_2 31
#define MainMotorPin2_1 32
#define MainMotorPin2_2 33
#define MainMotorPin3_1 34
#define MainMotorPin3_2 35
#define MainMotorPin4_1 36
#define MainMotorPin4_2 37
extern float TargetFrequency[4];

//dribbler motor
#define DribblerMotorpin1 1
#define DribblerMotorpin2 1

//ball sensor
#define BallI2C i2c0
#define BallSDApin 16
#define BallSCLpin 17
extern float BallAngle;  //時計回り0～360° 999.0でボール持ってる。-999.0で行方不明
extern int BallDistance;

//camera
#define CameraUART uart1
#define CameraTXpin 4
#define CameraRXpin 5
struct GoalInformation{
    int X;
    int Y;
    float angle; //正面が0度で時計回りの角度
    float distance;
};
extern struct GoalInformation MyGoal;
extern struct GoalInformation OpponentGoal;
extern int LeftWall, RightWall;

//display
#define DisplayI2C i2c1  //i2c0
#define DisplaySDApin 14 //16
#define DisplaySCLpin 15 //17
#define isUseDisplay true
#define DisplayBufferSize 200
extern char DisplayBuffer[DisplayBufferSize];
extern int DisplayMode;

//gyro sensor
#define GyroI2C i2c1
#define GyroSDApin 14 //サブマイコンに追放されました
#define GyroSCLpin 15

extern float AngleX, AngleY, AngleZ; //時計回り0～360°

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

//motor
#define PWMpin1 6
#define PWMpin2 7
#define PWMpin3 8
#define PWMpin4 10

//UART for RP2040
#define RP2040_UART_TXpin 43 //J12の右端
#define RP2040_UART_RXpin 44
#define RP2040_UART_IRQpin 45
#define SERIAL_BAUD 125000

//encoder
extern bool isMotorClockWise[4];
extern float motorFrequency[4];

//others
