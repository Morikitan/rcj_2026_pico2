#include "action.hpp"
#include "ball.hpp"
#include "camera.hpp"
#include "defence.hpp"
#include "display.hpp"
#include "gyro.hpp"
#include "line.hpp"
#include "motor.hpp"
#include "../config.hpp"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

int DoneLineSensor[48];
float Vector[24];
int Weight[24];
int VectorNumber;
float DefenceAngle = 0.0; //時計回り0～360°
float preDefenceAngle = 0.0;
float DefenceBallTime = 0.0;
float VectorAbsoluteValue;

void Defence(){
  UseAllSensor();

  DefenceAngle = GetCircleLineVector();
  if(DefenceAngle != 999.9) preDefenceAngle = DefenceAngle;

  if(BallAngle == 999.0){
      if(time_us_32() / 1000000.0 - DefenceBallTime > 1){
        while(time_us_32() / 1000000.0 - DefenceBallTime < 2 && (mode == 3 || mode == 4)){
          UseGyroSensor();
          TargetFrequency[0] = DefaultFrequency;
          TargetFrequency[1] = DefaultFrequency;
          TargetFrequency[2] = DefaultFrequency;
          TargetFrequency[3] = DefaultFrequency;
          Turn();
          EncoderAllMainMotorState(TargetFrequency);
        }
        DefenceBallTime = time_us_32() / 1000000.0;
      }
  }else{
    DefenceBallTime = time_us_32() / 1000000.0;
    if(BallAngle == -999.0){
      //中央に移動
    }else{
      if(DefenceAngle == 999.9){
        //ラインの真ん中にいるとき
        DefenceAngle = preDefenceAngle;
      }
      if(DefenceAngle == -999.9 || AllLineSensor <= ErorrLineSensor){
        //ラインの上にいないとき
        if(MyGoal.distance < GoalDistance){
          //ゴールの中にいる
          TargetFrequency[0] = DefaultFrequency * -cos(radian45(MyGoal.angle));
          TargetFrequency[1] = DefaultFrequency * -sin(radian45(MyGoal.angle));
          TargetFrequency[2] = DefaultFrequency * -cos(radian45(MyGoal.angle));
          TargetFrequency[3] = DefaultFrequency * -sin(radian45(MyGoal.angle));
          Turn();
          EncoderAllMainMotorState(TargetFrequency);
        }else{
          //ゴールの外にいる
          if(MyGoal.distance == 999.0){
            //自ゴールが見えない→ディフェンス復帰
            DefenceStart();
          }else{
            TargetFrequency[0] = DefaultFrequency * cos(radian45(MyGoal.angle));
            TargetFrequency[1] = DefaultFrequency * sin(radian45(MyGoal.angle));
            TargetFrequency[2] = DefaultFrequency * cos(radian45(MyGoal.angle));
            TargetFrequency[3] = DefaultFrequency * sin(radian45(MyGoal.angle));
            Turn();
            EncoderAllMainMotorState(TargetFrequency);
          }
        }
      }else{
        //ラインの上にいるとき
        
        if(15 < AngleX  && AngleX < 345){
          //回転を優先
          TurnToTargetAngle(0.0,false);
        }else if(MyGoal.angle < 90 || 270 < MyGoal.angle){
          //???????????????????????
        }else{
          //ラインに対して平行成分と垂直成分を足し算する
          //基本的にボールとゴールの直線上に機体がいることを目指す
          
          //平行成分
          if(BallAngle <= 180){
            BallAngle += 180;
          }else{
            BallAngle -= 180;
          }

          if(MyGoal.angle <= BallAngle){
            //直線より左側にいる
            TargetFrequency[0] += DefaultFrequency * (0.2 + (BallAngle - MyGoal.angle) / 120.0) * fabs(cos(radian45(DefenceAngle)));
            TargetFrequency[1] -= DefaultFrequency * (0.2 + (BallAngle - MyGoal.angle) / 120.0) * fabs(sin(radian45(DefenceAngle)));
            TargetFrequency[2] += DefaultFrequency * (0.2 + (BallAngle - MyGoal.angle) / 120.0) * fabs(cos(radian45(DefenceAngle)));
            TargetFrequency[3] -= DefaultFrequency * (0.2 + (BallAngle - MyGoal.angle) / 120.0) * fabs(sin(radian45(DefenceAngle)));
          }else{
            //直線より右側にいる
            TargetFrequency[0] -= DefaultFrequency * (0.2 + (MyGoal.angle - BallAngle) / 120.0) * fabs(cos(radian45(DefenceAngle)));
            TargetFrequency[1] += DefaultFrequency * (0.2 + (MyGoal.angle - BallAngle) / 120.0) * fabs(sin(radian45(DefenceAngle)));
            TargetFrequency[2] -= DefaultFrequency * (0.2 + (MyGoal.angle - BallAngle) / 120.0) * fabs(cos(radian45(DefenceAngle)));
            TargetFrequency[3] += DefaultFrequency * (0.2 + (MyGoal.angle - BallAngle) / 120.0) * fabs(sin(radian45(DefenceAngle)));
          }

          //垂直成分
          TargetFrequency[0] += DefaultFrequency * 0.3 * VectorAbsoluteValue * cos(radian45(DefenceAngle));
          TargetFrequency[1] += DefaultFrequency * 0.3 * VectorAbsoluteValue * sin(radian45(DefenceAngle));
          TargetFrequency[2] += DefaultFrequency * 0.3 * VectorAbsoluteValue * cos(radian45(DefenceAngle));
          TargetFrequency[3] += DefaultFrequency * 0.3 * VectorAbsoluteValue * sin(radian45(DefenceAngle));
        }
        MyGoal.angle == BallAngle + 180;
      }
    }
  }
}

//ディフェンスの初期移動に戻る
void DefenceStart(){

}

//踏んでいるラインの形が直線か、直角かを判別する
void JudgeLineShape(){
    
}

float result;
//最後は時計回りの0～360で出力
float GetCircleLineVector(){
    //データを使える形に変換する
    for(int i = 0;i < 48;i++){
      DoneLineSensor[i] = false;
      if(i < 24){
        Vector[i] = 0;
        Weight[i] = 0;
      }
    }  
    VectorNumber = 0;
    for(int i = 0;i < 48;i++){
      if(LineSensorE[i] > 0 && DoneLineSensor[i] == false){
        if(i == 0){
          //LineSensor[0]だけ反時計回り側にあるセンサを考える
          int k = 47;
          while(k >= 1 && LineSensorE[k] > 0){
            DoneLineSensor[k] = true;
            k--;
          }
          Vector[VectorNumber] -= (47 - k) * 3.75;
          Weight[VectorNumber] += 47 - k;
        }
        int j = 1;
        while(i + j <= 47 && LineSensorE[i+j] > 0){
            DoneLineSensor[i + j] = true;
            j++;
        }
        Vector[VectorNumber] += (j - 1)*3.75 + 7.5 * i;
        Weight[VectorNumber] += j;

        VectorNumber++;
        DoneLineSensor[i] = true;
        if(VectorNumber >= 24) break;
      }
    }

    //ベクトルの合成をする
  float VectorX = 0;
  float VectorY = 0;
  for(int i = 0;i < VectorNumber;i++){
    VectorX -= sin(Vector[i] / 180.0 * 3.1415);
    VectorY += cos(Vector[i] / 180.0 * 3.1415);
    if(SerialWatch == "vec" || SerialWatch == "lin"){
      printf("%d : %f ",i,Vector[i]);
    }
  }
  if(VectorNumber == 0){
    VectorX = 999;
    VectorY = 999;
  }else{
    VectorX /= (float)VectorNumber;
    VectorY /= (float)VectorNumber;
  }
  result = atan2(VectorY,VectorX) / 3.1415 * -180 + 90 - 3.75;
  while(result < 0) result += 360.0;
  while(result >= 360) result -= 360.0;
  VectorAbsoluteValue = sqrt(VectorX * VectorX + VectorY * VectorY);

  if(SerialWatch == "vec"){
    printf(" 向き : ");
    if(VectorX == 999 && VectorY == 999){
      printf("ラインの上にいない!!\n");
    }else if(VectorX == 0 && VectorY == 0){
      printf("真ん中\n");
    }else{
      printf("%f\n",result);
    }
  }
  //-π～πまででもとめられる。正面がπ/2になる
  if(VectorX == 999 && VectorY == 999){
    return -999.9;
  }else if(VectorX == 0 && VectorY == 0){
    return 999.9;
  }else{
    return result;
  }
}

float _TurnFrequency;
//正面を向くための機体の回転方向の回転数を加算する
void Turn(){
  if (AngleX > 180) {
    if(AddTurnFrequency * (360 - AngleX) / 180 < MaxAddTurnFrequency){
      _TurnFrequency = TurnFrequency * (360 - AngleX) / 180;
    }else{
      _TurnFrequency = MaxAddTurnFrequency;
    }
  } else {
    if(AddTurnFrequency * AngleX / 180 < MaxAddTurnFrequency){
      _TurnFrequency = AddTurnFrequency * AngleX / 180 * -1;
    }else{
      _TurnFrequency = -MaxAddTurnFrequency;
    }
  }
  TargetFrequency[0] += _TurnFrequency;
  TargetFrequency[1] += _TurnFrequency;
  TargetFrequency[2] -= _TurnFrequency;
  TargetFrequency[3] -= _TurnFrequency;
}
