#include "defence.hpp"
#include "action.hpp"
#include "motor.hpp"
#include "line.hpp"
#include "gyro.hpp"
#include "camera.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <math.h>

int DoneLineSensor[48];
float Vector[24];
int Weight[24];
int VectorNumber;

void Defence(){

}

//ディフェンスの初期移動に戻る
void DefenceStart(){

}

//踏んでいるラインの形が直線か、直角かを判別する
void JudgeLineShape(){
    
}

float result;
//反時計回りに配置されているうえに0のいちが3.75度ずれていることが発覚いたしました。
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
  float VectorAbsoluteValue = sqrt(VectorX * VectorX + VectorY * VectorY);

  if(SerialWatch == "vec"){
    printf(" 向き(真右が0度) : ");
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
