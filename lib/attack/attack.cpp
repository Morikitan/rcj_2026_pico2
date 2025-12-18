#include "attack.hpp"
#include "action.hpp"
#include "ball.hpp"
#include "camera.hpp"
#include "defence.hpp"
#include "display.hpp"
#include "gyro.hpp"
#include "line.hpp"
#include "motor.hpp"
#include "rp2040.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <math.h>
#include <stdio.h>


float AngleFrequency = 0;
float AngleFrequencyI = 0;
float LastATime = -2;
float LastCTime = -2;
uint32_t BallPreTime = 0;
uint32_t MainPreTime = 0;
uint32_t PreTime1;
bool isBreak = false;
bool isTargetFrequencyLine = false;

void Attack(){
    //ライン上に乗っているかを判定する
    NewLineMove();
    UseGyroSensor();
    UseBallSensor();
    UseCamera();
    if (BallAngle == 999) {
      //gpio_put(Bupin,0);
      if((time_us_32() - PreTime1) / 1000000.0 > 0.2){
        isTargetFrequencyLine = false;
        //マカオシュートの準備～実行
        //反転してるときはカメラの向きが変わる
        if(OpponentGoal.angle != 999 &&((AngleX <= 45 && 45 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 180 - AngleX) ||
         (45 < AngleX && AngleX <= 180 &&             ((405 - AngleX < OpponentGoal.angle && OpponentGoal.angle <= 360)||(0 <= OpponentGoal.angle && OpponentGoal.angle < 180 - AngleX))) ||
         (AngleX > 180 &&                               405 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 540 - AngleX) )){
          //ゴールの左側の奥?
          isBreak = TurnToTargetAngle(245.0,true);
          if(isBreak == false) Makao(false,165);
        }else if(OpponentGoal.angle != 999 && ((AngleX <= 180 && 180 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 315 - AngleX) ||
         (180 < AngleX && AngleX <= 315 && ((540 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 360) || 0 < OpponentGoal.angle && OpponentGoal.angle < 315 - AngleX)) ||
         (AngleX > 315 &&                    540 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 675 - AngleX) )){
          //ゴールの右側の奥
          isBreak = TurnToTargetAngle(115.0,true);
          if(isBreak == false) Makao(true,195);
        }else if(OpponentGoal.distance < 110){
          isBreak = false;
          if (((AngleX < 180  && 180 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX)
          ||  (AngleX >= 180 && (540 - AngleX < OpponentGoal.angle && OpponentGoal.angle <= 360) || (0 < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX)))) {
            //ゴールの左側にいるとき
            isBreak = TurnToTargetAngle(180.0,true);
            if(isBreak == false) Makao(false,80);  
          } else{
            //ゴールの右側にいるとき
            isBreak = TurnToTargetAngle(180.0,true);
            if(isBreak == false)Makao(true,280);
          }
          //gpio_put(Bupin,0);
        }else{
          if(OpponentGoal.angle < 180){
            //ゴールが右側にある
              ChaseBall(OpponentGoal.angle - 20,true);
          }else if(180 < OpponentGoal.angle && OpponentGoal.angle <= 360){
            //ゴールが左側にある
              ChaseBall(OpponentGoal.angle + 20,true);
          }else{
            //ゴールが遠すぎる
            if(LeftWall < RightWall){
              //ゴールが右側にある
              ChaseBall(AngleX * -1 - 20,true);
            }else{
              //ゴールが左側にある
              ChaseBall(AngleX * -1 + 20,true);
            }
          }
        }
      }
    } else {
      //gpio_put(Bupin,0);
      PreTime1 = time_us_32();
      //ボールを拾いに行く
      if (BallAngle == -999) {
        //ボールがどこかわからない
        while(BallAngle == -999){
          UseBallSensor();
          EncoderAllMainMotorState((float[]){0.0,0.0,0.0,0.0});
        }
      } else {
        //ボールのほうへ全速前進
        if (BallAngle >= 180) {
          BallAngle -= 360;
        } else if (BallAngle < -180) {
          BallAngle += 360;
        }
        
        /*if (BallDistance == 4) {
          ChaseBall(BallAngle,false);
        } else if (BallDistance == 3) {
          ChaseBall(BallAngle * 1.25,false);
        } else */{
          if ((-60 <= BallAngle && BallAngle <= 60) || (300 <= BallAngle && BallAngle <= 420)) {
            ChaseBall(BallAngle * 1.45,false);
          } else {
            ChaseBall(BallAngle * 1.55,false);
          }
        }
      }
    }
}

void NewLineMove(){
    UseLineSensor();
    if(AllLineSensor > 0){
      float LineDeltaTime = 0.0;
      float DeltaTime = 0.0;
      float FirstAngle;  //最初に踏んだラインの向き
      float LineAngle; //円形ラインセンサの合成ベクトルの向き
      Brake();
      int LineDuty[4];
      if(AllLineSensorA > 0){
        FirstAngle = 0.0;
      }else if(AllLineSensorB > 0){
        FirstAngle = 270.0;
      }else if(AllLineSensorC > 0){
        FirstAngle = 180.0;
      }else if(AllLineSensorD > 0){
        FirstAngle = 90.0;
      }else{
        //LineSensorEが先に反応したケース
        FirstAngle = GetCircleLineVector();
      }
      LineDuty[0] = LineFrequency * (-sin((FirstAngle - 90.0) / 180.0 * -3.1415) - cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
      LineDuty[1] = LineFrequency * (-sin((FirstAngle - 90.0) / 180.0 * -3.1415) + cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
      LineDuty[2] = LineFrequency * (-sin((FirstAngle - 90.0) / 180.0 * -3.1415) - cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
      LineDuty[3] = LineFrequency * (-sin((FirstAngle - 90.0) / 180.0 * -3.1415) + cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
      MainPreTime = time_us_32() / 1000000.0;
      while (LineDeltaTime < 0.03) {
        UseLineSensor();
        UseGyroSensor();
        if (AllLineSensor <= ErorrLineSensor) {
          LineDeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        } else {
          LineDeltaTime = 0;
        }
        DeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        MainPreTime = time_us_32() / 1000000.0;
        LineAngle = GetCircleLineVector();
        //-999.9の時は中央なので何もしない
        if(LineAngle != -999.9 && (LineAngle == 999.9 || (FirstAngle < 90.0 && FirstAngle + 90 < LineAngle && LineAngle < FirstAngle + 270) || (90 <= FirstAngle && FirstAngle <= 270 && (LineAngle < FirstAngle - 90 || FirstAngle + 90 < LineAngle)) || (270 < FirstAngle && FirstAngle - 270 <= LineAngle && LineAngle <= FirstAngle - 90))){
          //円形が反応しないか、最初の向きと逆の時
          TargetFrequency[0] = LineDuty[0];
          TargetFrequency[1] = LineDuty[1];
          TargetFrequency[2] = LineDuty[2];
          TargetFrequency[3] = LineDuty[3];
        }else if(LineAngle != -999.9){
          TargetFrequency[0] = LineFrequency * (-sin((LineAngle - 90.0) / 180.0 * -3.1415) - cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
          TargetFrequency[1] = LineFrequency * (-sin((LineAngle - 90.0) / 180.0 * -3.1415) + cos((FirstAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
          TargetFrequency[2] = LineFrequency * (-sin((LineAngle - 90.0) / 180.0 * -3.1415) - cos((LineAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
          TargetFrequency[3] = LineFrequency * (-sin((LineAngle - 90.0) / 180.0 * -3.1415) + cos((LineAngle - 90.0) / 180.0 * -3.1415)) / 1.4142;
        }
        Turn();
        EncoderAllMainMotorState(TargetFrequency);
      }
    }
}

void LineMove(){
    UseLineSensor();
    if (AllLineSensor > 0){
      float LineDeltaTime = 0.0;
      float DeltaTime = 0.0;
      Brake();
      int LineDuty[4];
      {
        //反応したラインセンサから遠ざかるように進む
        if(AllLineSensorA + LineSensorE[15] + LineSensorE[0] + LineSensorE[1] > 0 || (AllLineSensorC + LineSensorE[7] + LineSensorE[8] + LineSensorE[9] > 0 && time_us_32() / 1000000.0 - LastATime < 1.5 )){
          LastATime = time_us_32() / 1000000.0;
          if(45 <= OpponentGoal.angle && OpponentGoal.angle <= 110 || LeftWall <= 71){
            //左端
            LineDuty[0] = -LineFrequency / 2;
            LineDuty[1] = -LineFrequency;
            LineDuty[2] = -LineFrequency / 2;
            LineDuty[3] = -LineFrequency;
          }else if(250 <= OpponentGoal.angle && OpponentGoal.angle <= 315 || RightWall <= 55){
            //右端
            LineDuty[0] = -LineFrequency;
            LineDuty[1] = -LineFrequency / 2;
            LineDuty[2] = -LineFrequency;
            LineDuty[3] = -LineFrequency / 2;
          }else{
            LineDuty[0] = -LineFrequency;
            LineDuty[1] = -LineFrequency;
            LineDuty[2] = -LineFrequency;
            LineDuty[3] = -LineFrequency;
          }
        }else if(AllLineSensorB + LineSensorE[3] + LineSensorE[4] + LineSensorE[5] > 0){
          if(OpponentGoal.distance <= 200){
            //相手のゴール際で事故らないようにするため
            LineDuty[0] = LineFrequency / 2;
            LineDuty[1] = -LineFrequency;
            LineDuty[2] = LineFrequency / 2;
            LineDuty[3] = -LineFrequency;
          }else if(MyGoal.distance <= 200){
            LineDuty[0] = LineFrequency;
            LineDuty[1] = -LineFrequency / 2;
            LineDuty[2] = LineFrequency;
            LineDuty[3] = -LineFrequency / 2;
          }else{
            LineDuty[0] = (int)(LineFrequency / 1.5);
            LineDuty[1] = -LineFrequency;
            LineDuty[2] = (int)(LineFrequency / 1.5);
            LineDuty[3] = -LineFrequency;
          }
        }else if(AllLineSensorC + LineSensorE[7] + LineSensorE[8] + LineSensorE[9] > 0 || (AllLineSensorA + LineSensorE[15] + LineSensorE[0] + LineSensorE[1] > 0 && time_us_32() / 1000000.0 - LastCTime < 1.5 )){
          LastCTime = time_us_32() / 1000000.0;
          if((80 <= MyGoal.angle && MyGoal.angle <= 135) || LeftWall <= 71){
            //左端
            LineDuty[0] = LineFrequency;
            LineDuty[1] = LineFrequency / 2;
            LineDuty[2] = LineFrequency;
            LineDuty[3] = LineFrequency / 2;
          }else if((225 <= MyGoal.angle && MyGoal.angle <= 280) || RightWall <= 55){
            //右端
            LineDuty[0] = LineFrequency / 2;
            LineDuty[1] = LineFrequency;
            LineDuty[2] = LineFrequency / 2;
            LineDuty[3] = LineFrequency;
          }else{
            LineDuty[0] = LineFrequency;
            LineDuty[1] = LineFrequency;
            LineDuty[2] = LineFrequency;
            LineDuty[3] = LineFrequency;
          }
        }else if(AllLineSensorD + LineSensorE[11] + LineSensorE[12] + LineSensorE[13] > 0){
          if(OpponentGoal.distance <= 200){
            //相手のゴール際で事故らないようにするため
            LineDuty[0] = -LineFrequency;
            LineDuty[1] = LineFrequency / 2;
            LineDuty[2] = -LineFrequency;
            LineDuty[3] = LineFrequency / 2;
          }else if(MyGoal.distance <= 200){
            LineDuty[0] = -LineFrequency / 2;
            LineDuty[1] = LineFrequency;
            LineDuty[2] = -LineFrequency / 2;
            LineDuty[3] = LineFrequency;
          }else{
            LineDuty[0] = -LineFrequency;
            LineDuty[1] = (int)(LineFrequency / 1.5);
            LineDuty[2] = -LineFrequency;
            LineDuty[3] = (int)(LineFrequency / 1.5);
          }
        }
      }
      TargetFrequency[0] = LineDuty[0];
      TargetFrequency[1] = LineDuty[1];
      TargetFrequency[2] = LineDuty[2];
      TargetFrequency[3] = LineDuty[3];
      EncoderAllMainMotorState(TargetFrequency);
      MainPreTime = time_us_32() / 1000000.0;
      while (LineDeltaTime < 0.3) {
        UseAllSensor();
        if (AllLineSensor <= ErorrLineSensor) {
          LineDeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        } else {
          LineDeltaTime = 0;
        }
        DeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        MainPreTime = time_us_32() / 1000000.0;
      }
    }
}

void ChaseBall(float angle,bool isMakao){
  if(isMakao == true){
    if(((AngleX < 180  && 180 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX && OpponentGoal.angle != 999)
        ||  (AngleX >= 180 && (540 - AngleX < OpponentGoal.angle && OpponentGoal.angle <= 360) || (0 < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX) && OpponentGoal.angle != 999)
        ||  (OpponentGoal.angle == 999 && LeftWall < RightWall))  )
    {
      //ゴールの左側にいる
      if(180 < AngleX && AngleX < 210){
        if (AddTurnFrequency * (AngleX - 180) / 180 > MaxAddTurnFrequency) {
          AngleFrequency = -MaxAddTurnFrequency;
        } else {
          AngleFrequency = AddTurnFrequency * (AngleX - 180) / 180 * -1;
        }
      }else{
        if(180 < AngleX){
          AngleFrequency = MaxAddTurnFrequency;
        }else{
          if (AddTurnFrequency * (180 - AngleX) / 180 > MaxAddTurnFrequency) {
            AngleFrequency = MaxAddTurnFrequency;
          } else {
            AngleFrequency = AddTurnFrequency * (180 - AngleX) / 180;
          }
        }
      }
    }else{
      if(150 < AngleX && AngleX < 180){
        if (AddTurnFrequency * (180 - AngleX) / 180 > MaxAddTurnFrequency) {
          AngleFrequency = MaxAddTurnFrequency;
        } else {
          AngleFrequency = AddTurnFrequency * (180 - AngleX) / 180;
        }
      }else{
        if(AngleX >= 180){
          if (AddTurnFrequency * (AngleX - 180) / 180 > MaxAddTurnFrequency) {
            AngleFrequency = -MaxAddTurnFrequency;
          } else {
            AngleFrequency = AddTurnFrequency * (AngleX - 180) / 180 * -1;
          }
        }else{
          AngleFrequency = -MaxAddTurnFrequency;
        }
        
      } 
    }
  }else{
    //ボールを持っていない通常時
    if (AngleX > 180) {
      if (AddTurnFrequency * (360 - AngleX) / 180 > MaxAddTurnFrequency) {
        AngleFrequency = MaxAddTurnFrequency;
      } else {
        AngleFrequency = TurnFrequency * (360 - AngleX) / 180;
      }
    } else {
      if (AddTurnFrequency * AngleX / 180 > MaxAddTurnFrequency) {
        AngleFrequency = -MaxAddTurnFrequency;
      } else {
        AngleFrequency = AddTurnFrequency * AngleX / 180 * -1;
      }
    }
  }
  float SinFrequency2 = 0;
  float Gensoku;
  //ボールを持っているときに左右の動きの動きをつける
  //if(isMakao == true)SinFrequency2 = SinFrequency * sin(BallPreTime / 1000000.0);
  //壁際に近い時は減速する
  if((0 < LeftWall && LeftWall < 100) || (0 < RightWall && RightWall < 60)) Gensoku = 0.75;
  else Gensoku = 1.0;

  TargetFrequency[0] = (MaxFrequency * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleFrequency + SinFrequency2);
  TargetFrequency[1] = (MaxFrequency * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleFrequency - SinFrequency2);
  TargetFrequency[2] = (MaxFrequency * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleFrequency + SinFrequency2);
  TargetFrequency[3] = (MaxFrequency * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleFrequency - SinFrequency2);

  EncoderAllMainMotorState(TargetFrequency);
  BallPreTime = time_us_32();
}

void NonDribbler(float angle,bool isClockWise){
  if(isClockWise == true){
    if (AngleX > 210) {
        AngleFrequency = MaxTurnFrequency;
    } else if(AngleX > 30){
        AngleFrequency = -MaxTurnFrequency;
    }else{
        AngleFrequency = MaxTurnFrequency;
    }
  }else{
    if (AngleX > 330) {
       AngleFrequency = -MaxTurnFrequency;
    } else if(AngleX > 150){
      
        AngleFrequency = MaxTurnFrequency;
      
    }else{
        AngleFrequency = -MaxTurnFrequency;
      
    }
  }
  float SinFrequency2;
  float Gensoku;
  //ボールを持っているときに左右の動きの動きをつける
  SinFrequency2 = 0;
  //壁際に近い時は減速する
  if(LeftWall < GensokuDistance || RightWall < GensokuDistance) Gensoku = 0.75;
  else Gensoku = 1.0;

  TargetFrequency[0] = (MaxFrequency * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleFrequency + SinFrequency2);
  TargetFrequency[1] = (MaxFrequency * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleFrequency - SinFrequency2);
  TargetFrequency[2] = (MaxFrequency * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleFrequency + SinFrequency2);
  TargetFrequency[3] = (MaxFrequency * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleFrequency - SinFrequency2);

  EncoderAllMainMotorState(TargetFrequency);

  BallPreTime = time_us_32();
}

void Makao(bool isClockWise,int TargetAngle){
  // Brake();
  // sleep_ms(2000);
  //isBreakのリセットは不要
  if(isClockWise == true){
    while (TargetAngle - 150 < AngleX && AngleX <= TargetAngle) {
      UseLineSensor();
      UseGyroSensor();
      EncoderAllMainMotorState((float[]){MakaoFrequency-1.0,MakaoFrequency-1.0,MakaoFrequency,MakaoFrequency});
      if(AngleX > TargetAngle - 10){
        // BLDCState(1000);
      }
      if(AllLineSensor > ErorrLineSensor){
        isBreak = true;
        break;
      }
    }
    if(isBreak == false){
      Brake();
      sleep_ms(100);
      TurnToTargetAngle(0.0,false);
    }
  }else{
    while (TargetAngle <= AngleX && AngleX < TargetAngle + 150) {
      UseLineSensor();
      UseGyroSensor();
      EncoderAllMainMotorState((float[]){-MakaoFrequency,-MakaoFrequency,-(MakaoFrequency-1.0),-(MakaoFrequency-1.0)});
      if(AngleX < TargetAngle + 10){
        //BLDCState(1000);
      }
      if(AllLineSensor > ErorrLineSensor){
        isBreak = true;
        break;
      }
    } 
    if(isBreak == false){
      Brake();
      sleep_ms(100);
      TurnToTargetAngle(0.0,true);
    }
  }

  if(mode == 9 || mode == 10){
    mode -= 6;
    DefenceStart();
  }
}
