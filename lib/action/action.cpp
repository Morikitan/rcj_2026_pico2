#include "action.hpp"
#include "ball.hpp"
#include "camera.hpp"
#include "display.hpp"
#include "gyro.hpp"
#include "line.hpp"
#include "motor.hpp"
#include "../config.hpp"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

std::string SerialWatch;
int mode;
char isYellowMyGoal;
float TargetFrequency[4];
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
char DisplayBuffer[DisplayBufferSize];
int DisplayMode;

//変数の初期化
void VariableSetup(){
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
    SerialWatch = "hom";
    DisplayMode = 1;//上のコメントでの一番左側の数字

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

//正面0度時計回りの度数法の角度を座標平面の弧度法(正面π/2反時計回り)に変換する。
//定義域は 0 <= θ < 2π
float radian(float angle){
    if(angle <= 90){
        return (angle * -1.0 + 90) * 3.1415 / 180;
    }else{
        return (angle * -1.0 + 450) * 3.1415 / 180;
    }
}

//正面0度時計回りの度数法の角度を座標平面の弧度法(ただし正面π/4反時計回り)に変換する。
//つまり正面が通常より時計回りに45度回転した角度になる
//定義域は 0 <= θ < 2π
float radian45(float angle){
    if(angle <= 45){
        return (angle * -1.0 + 45) * 3.1415 / 180;
    }else{
        return (angle * -1.0 + 405) * 3.1415 / 180;
    }
}

//最高速度を測定するプログラム
void MeasureMaxSpeed(){

}

