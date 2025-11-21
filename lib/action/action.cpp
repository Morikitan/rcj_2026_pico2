#include "action.hpp"
#include "motor.hpp"
#include "ball.hpp"
#include "camera.hpp"
#include "display.hpp"
#include "gyro.hpp"
#include "line.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string>

std::string SerialWatch;
int mode;
char isYellowMyGoal;
int MotorDuty[4];
float BallAngle;  //999.0でボール持ってる。-999.0で行方不明
int BallDistance;
struct GoalInformation MyGoal;
struct GoalInformation OpponentGoal;
int LeftWall, RightWall;
float AngleX, AngleY, AngleZ;
char LineSensorA[5];
char LineSensorB[9];
char LineSensorC[9];
char LineSensorD[9];
char LineSensorE[48];
int AllLineSensorA, AllLineSensorB, AllLineSensorC, AllLineSensorD, AllLineSensorE, AllLineSensor;
int ErorrLineSensor;
bool isMotorClockWise[4];
float motorFrequency[4];

//変数の初期化
void VariableSetup(){
    /******************
    ang 機体の角度(AngleX)
    bal ボールセンサーの値(BallDistance,BallAngle)
    cam カメラの値(自分と敵のゴールの距離と向き、壁との距離)
    lin ラインセンサーの値(0か1で受け取る)
    mot モーターに加わる電圧(AngleXもセットで)
    tim 1回の経過時間(ミリ秒)
    *******************/
    SerialWatch = "ang";

    //必ず変更しましょう。1で黄色ゴールが自分側(相手にシュートされる側)。0で逆
    isYellowMyGoal = 1;

    //mode
    mode = 0;  //mode = 99で全部正転

    //gyro sensor
    AngleX = 0; 
    AngleY = 0; 
    AngleZ = 0;

    //line sensor        
    ErorrLineSensor = 1;

    //camera
    LeftWall = 0;
    RightWall = 0;
}

//その他のピンの初期化
void PinSetup(){

}

//足回りモーターすべてに、最大の強さのブレーキをかける。
void Brake(){
    MainMotorState(1,3,255);
    MainMotorState(2,3,255);
    MainMotorState(3,3,255);
    MainMotorState(4,3,255);
}

//MotorDuty配列の値をMainMotorStateで出力する。
void UseMotorDuty(){
    for (int a = 0; a <= 3; a++) {
        if (MotorDuty[a] > 255) {
            MainMotorState(a + 1, 0, 255);
        } else if (MotorDuty[a] > 0) {
            MainMotorState(a + 1, 0, (int)(MotorDuty[a]));
        } else if (MotorDuty[a] > -255) {
            MainMotorState(a + 1, 1, (int)(MotorDuty[a] * -1));
        } else {
            MainMotorState(a + 1, 1, 255);
        }
    }
}

//エンコーダー以外のすべてのセンサーを使用する。
void UseAllSensor(){
    UseBallSensor();
    UseCamera();
    UseGyroSensor();
    UseLineSensor();
}

#define TurnKp 0.10
#define TurnKi 0.00
#define TurnKd 0.00
float TurnPreTime = 0;
float TurnI = 0;
float TurnP = 0;
float TurnPreP = 0;
float OutputFrequency = 0; //上から見て時計回りが正
//特定の角度を向く関数
bool TurnToTargetAngle(float angle, bool isUseIsBreak){
    while((angle <= 5 && (angle + 5 < AngleX && AngleX < angle + 355)) ||
          (5 < angle && angle <= 355 && (AngleX < angle - 5 || angle + 5 < AngleX)) ||
          (355 <= angle && (angle - 355 < AngleX && AngleX < angle - 5)))
    {   
        UseGyroSensor();
        if(isUseIsBreak == true){
            UseLineSensor();
            if(AllLineSensor > ErorrLineSensor){
                return true;
            }
        }
        float now = time_us_64() / 1000000.0;
        float dt = now - TurnPreTime;
        if(now - TurnPreTime > 1){
            //前回の処理から時間が大きくたっているときはいったんリセットする
            TurnI = 0;
            TurnPreP = 0;
        }
        //Proportional項の計算
        TurnP = angle - AngleX;
        while(TurnP > 180) TurnP -= 360;
        while(TurnP < -180) TurnP += 360;
        //Integral項の計算
        TurnI += TurnP * dt;
        //出力の決定
        OutputFrequency = TurnP * TurnKp + TurnI * TurnKi + (TurnP - TurnPreP) * TurnKd / dt;
        if(OutputFrequency > MaxFrequency){
            OutputFrequency = MaxFrequency;
            TurnI = 0;
        } 
        if(OutputFrequency < -MaxFrequency) {
            OutputFrequency = -MaxFrequency; 
            TurnI = 0;
        }

        TurnPreTime = now;
        TurnPreP = TurnP;

        EncoderAllMainMotorState((float[]){OutputFrequency,OutputFrequency,-OutputFrequency,-OutputFrequency});
    }
    return false;
}