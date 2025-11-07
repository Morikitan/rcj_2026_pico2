#include "attack.hpp"
#include "action.hpp"
#include "motor.hpp"
#include "gyro.hpp"
#include "rp2040.hpp"
#include "line.hpp"
#include "camera.hpp"
#include "defence.hpp"
#include "ball.hpp"
#include "../config.hpp"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <math.h>

float AngleSpeed = 0;
float AngleSpeedI = 0;
float LastATime = -2;
float LastCTime = -2;
uint32_t BallPreTime = 0;
uint32_t MainPreTime = 0;
uint32_t PreTime1;
bool isBreak = false;
bool isMotorDutyLine = false;

int MakaoLeastSpeed = 120; // 過去は80

void Attack(){
    //ライン上に乗っているかを判定する
    NewLineMove();
    UseGyroSensor();
    UseBallSensor();
    UseCamera();
    if (BallAngle == 999) {
      //gpio_put(Bupin,0);
      if((time_us_32() - PreTime1) / 1000000.0 > 0.2){
        isMotorDutyLine = false;
      //マカオシュートの準備～実行
        //反転してるときはカメラの向きが変わる
        if(OpponentGoal.angle != 999 &&((AngleX <= 45 && 45 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 180 - AngleX) ||
         (45 < AngleX && AngleX <= 180 &&             ((405 - AngleX < OpponentGoal.angle && OpponentGoal.angle <= 360)||(0 <= OpponentGoal.angle && OpponentGoal.angle < 180 - AngleX))) ||
         (AngleX > 180 &&                               405 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 540 - AngleX) )){
          //ゴールの左側の奥?
          isBreak = false;
          while (AngleX < 235 || 255 < AngleX) {
            UseLineSensor();
            UseGyroSensor();
            if(55 < AngleX && AngleX < 235){
              MainMotorState(1, 0, LeastTurnSpeed - 30);
              MainMotorState(2, 0, LeastTurnSpeed - 30);
              MainMotorState(3, 1, LeastTurnSpeed - 30);
              MainMotorState(4, 1, LeastTurnSpeed - 30);
            }else{
              if(AngleX < 30 || 330 < AngleX){
                MainMotorState(1, 1, LeastTurnSpeed - 10);
                MainMotorState(2, 1, LeastTurnSpeed - 10);
                MainMotorState(3, 0, LeastTurnSpeed - 10);
                MainMotorState(4, 0, LeastTurnSpeed - 10);
              }else{
                MainMotorState(1, 1, LeastTurnSpeed - 30);
                MainMotorState(2, 1, LeastTurnSpeed - 30);
                MainMotorState(3, 0, LeastTurnSpeed - 30);
                MainMotorState(4, 0, LeastTurnSpeed - 30);
              }
            }
            if(AllLineSensor > ErorrLineSensor){
              isBreak = true;
              break;
            }
          }
          if(isBreak == false) Makao(false,165);
        }else if(OpponentGoal.angle != 999 && ((AngleX <= 180 && 180 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 315 - AngleX) ||
         (180 < AngleX && AngleX <= 315 && ((540 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 360) || 0 < OpponentGoal.angle && OpponentGoal.angle < 315 - AngleX)) ||
         (AngleX > 315 &&                    540 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 675 - AngleX) )){
          //ゴールの右側の奥
          isBreak = false;
          while (AngleX < 105 || 125 < AngleX) {
            UseLineSensor();
            UseGyroSensor();
            if(AngleX < 105 || 285 < AngleX){
              if(AngleX < 30 || 330 < AngleX){
                MainMotorState(1, 0, LeastTurnSpeed - 10);
                MainMotorState(2, 0, LeastTurnSpeed - 10);
                MainMotorState(3, 1, LeastTurnSpeed - 10);
                MainMotorState(4, 1, LeastTurnSpeed - 10);
              }else{
                MainMotorState(1, 0, LeastTurnSpeed - 30);
                MainMotorState(2, 0, LeastTurnSpeed - 30);
                MainMotorState(3, 1, LeastTurnSpeed - 30);
                MainMotorState(4, 1, LeastTurnSpeed - 30);
              }
            }else{
              MainMotorState(1, 1, LeastTurnSpeed - 30);
              MainMotorState(2, 1, LeastTurnSpeed - 30);
              MainMotorState(3, 0, LeastTurnSpeed - 30);
              MainMotorState(4, 0, LeastTurnSpeed - 30);
            }
            if(AllLineSensor > ErorrLineSensor){
              isBreak = true;
              break;
            }
          }
          if(isBreak == false) Makao(true,195);
        }else if(OpponentGoal.distance < 110){
          isBreak = false;
          if (((AngleX < 180  && 180 - AngleX < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX)
          ||  (AngleX >= 180 && (540 - AngleX < OpponentGoal.angle && OpponentGoal.angle <= 360) || (0 < OpponentGoal.angle && OpponentGoal.angle < 360 - AngleX)))) {
            //ゴールの左側にいるとき
            while (AngleX < 170 || 190 < AngleX) {
              UseLineSensor();
              UseGyroSensor();
              if(300 > AngleX && AngleX > 60){
                MainMotorState(1, 1, LeastTurnSpeed - 30);
                MainMotorState(2, 1, LeastTurnSpeed - 30);
                MainMotorState(3, 0, LeastTurnSpeed - 30);
                MainMotorState(4, 0, LeastTurnSpeed - 30);
              }else{
                MainMotorState(1, 1, LeastTurnSpeed - 10);
                MainMotorState(2, 1, LeastTurnSpeed - 10);
                MainMotorState(3, 0, LeastTurnSpeed - 10);
                MainMotorState(4, 0, LeastTurnSpeed - 10);
              }
              
              if(AllLineSensor > ErorrLineSensor){
                isBreak = true;
                break;
              }
            }
            if(isBreak == false) Makao(false,80);  
          } else{
            //ゴールの右側にいるとき
            while (AngleX < 170 || 190 < AngleX) {
              UseLineSensor();
              UseGyroSensor();
              if(300 > AngleX && AngleX > 60){
                MainMotorState(1, 0, LeastTurnSpeed - 30);
                MainMotorState(2, 0, LeastTurnSpeed - 30);
                MainMotorState(3, 1, LeastTurnSpeed - 30);
                MainMotorState(4, 1, LeastTurnSpeed - 30);
              }else{
                MainMotorState(1, 0, LeastTurnSpeed - 10);
                MainMotorState(2, 0, LeastTurnSpeed - 10);
                MainMotorState(3, 1, LeastTurnSpeed - 10);
                MainMotorState(4, 1, LeastTurnSpeed - 10);
              }
              if(AllLineSensor > ErorrLineSensor){
                isBreak = true;
                break;
              }
            }
            if(isBreak == false)Makao(true,280);
          }
          //gpio_put(Bupin,0);
        }else{
          if(OpponentGoal.angle < 180){
            //ゴールが右側にある
              ChaseBall(OpponentGoal.angle - 20,true);
          }else if(180 < OpponentGoal.angle && OpponentGoal.angle < 400){
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
          MainMotorState(1,2,0);
          MainMotorState(2,2,0);
          MainMotorState(3,2,0);
          MainMotorState(4,2,0);
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
        FirstAngle = 1.57;
        LineDuty[0] = -LineSpeed;
        LineDuty[1] = -LineSpeed;
        LineDuty[2] = -LineSpeed;
        LineDuty[3] = -LineSpeed;
      }else if(AllLineSensorB > 0){
        FirstAngle = 3.14;
        LineDuty[0] = LineSpeed;
        LineDuty[1] = -LineSpeed;
        LineDuty[2] = LineSpeed;
        LineDuty[3] = -LineSpeed;
      }else if(AllLineSensorC > 0){
        FirstAngle = -1.57;
        LineDuty[0] = LineSpeed;
        LineDuty[1] = LineSpeed;
        LineDuty[2] = LineSpeed;
        LineDuty[3] = LineSpeed;
      }else if(AllLineSensorD > 0){
        FirstAngle = 0;
        LineDuty[0] = -LineSpeed;
        LineDuty[1] = LineSpeed;
        LineDuty[2] = -LineSpeed;
        LineDuty[3] = LineSpeed;
      }
      MainPreTime = time_us_32() / 1000000.0;
      while (LineDeltaTime < 0.3) {
        UseLineSensor();
        UseGyroSensor();
        if (AllLineSensor <= ErorrLineSensor) {
          LineDeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        } else {
          LineDeltaTime = 0;
        }
        DeltaTime += time_us_32() / 1000000.0 - MainPreTime;
        MainPreTime = time_us_32() / 1000000.0;
        // LineAngle = GetLineAngle();
        //-999.9の時は中央なので何もしない
        if(LineAngle == 999.9 || (FirstAngle > 0 && (FirstAngle - 4.71 < LineAngle && LineAngle < FirstAngle - 1.57)) || (FirstAngle == 0 && (LineAngle < -1.57 || 1.57 < LineAngle)) || (FirstAngle < 0 && (FirstAngle + 1.57 < LineAngle && LineAngle < FirstAngle + 4.71))){
          //円形が反応しないか、最初の向きと逆の時
          MotorDuty[0] = LineDuty[0];
          MotorDuty[1] = LineDuty[1];
          MotorDuty[2] = LineDuty[2];
          MotorDuty[3] = LineDuty[3];
        }else if(LineAngle != -999.9){
          MotorDuty[0] = (int)(LineSpeed * (-cos(LineAngle) - sin(LineAngle)) / 1.2);
          MotorDuty[1] = (int)(LineSpeed * (cos(LineAngle) - sin(LineAngle)) / 1.2);
          MotorDuty[2] = (int)(LineSpeed * (-cos(LineAngle) - sin(LineAngle)) / 1.2);
          MotorDuty[3] = (int)(LineSpeed * (cos(LineAngle) - sin(LineAngle)) / 1.2);
        }
        // Turn();
        UseMotorDuty();
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
            LineDuty[0] = -LineSpeed / 2;
            LineDuty[1] = -LineSpeed;
            LineDuty[2] = -LineSpeed / 2;
            LineDuty[3] = -LineSpeed;
          }else if(250 <= OpponentGoal.angle && OpponentGoal.angle <= 315 || RightWall <= 55){
            //右端
            LineDuty[0] = -LineSpeed;
            LineDuty[1] = -LineSpeed / 2;
            LineDuty[2] = -LineSpeed;
            LineDuty[3] = -LineSpeed / 2;
          }else{
            LineDuty[0] = -LineSpeed;
            LineDuty[1] = -LineSpeed;
            LineDuty[2] = -LineSpeed;
            LineDuty[3] = -LineSpeed;
          }
        }else if(AllLineSensorB + LineSensorE[3] + LineSensorE[4] + LineSensorE[5] > 0){
          if(OpponentGoal.distance <= 200){
            //相手のゴール際で事故らないようにするため
            LineDuty[0] = LineSpeed / 2;
            LineDuty[1] = -LineSpeed;
            LineDuty[2] = LineSpeed / 2;
            LineDuty[3] = -LineSpeed;
          }else if(MyGoal.distance <= 200){
            LineDuty[0] = LineSpeed;
            LineDuty[1] = -LineSpeed / 2;
            LineDuty[2] = LineSpeed;
            LineDuty[3] = -LineSpeed / 2;
          }else{
            LineDuty[0] = (int)(LineSpeed / 1.5);
            LineDuty[1] = -LineSpeed;
            LineDuty[2] = (int)(LineSpeed / 1.5);
            LineDuty[3] = -LineSpeed;
          }
        }else if(AllLineSensorC + LineSensorE[7] + LineSensorE[8] + LineSensorE[9] > 0 || (AllLineSensorA + LineSensorE[15] + LineSensorE[0] + LineSensorE[1] > 0 && time_us_32() / 1000000.0 - LastCTime < 1.5 )){
          LastCTime = time_us_32() / 1000000.0;
          if((80 <= MyGoal.angle && MyGoal.angle <= 135) || LeftWall <= 71){
            //左端
            LineDuty[0] = LineSpeed;
            LineDuty[1] = LineSpeed / 2;
            LineDuty[2] = LineSpeed;
            LineDuty[3] = LineSpeed / 2;
          }else if((225 <= MyGoal.angle && MyGoal.angle <= 280) || RightWall <= 55){
            //右端
            LineDuty[0] = LineSpeed / 2;
            LineDuty[1] = LineSpeed;
            LineDuty[2] = LineSpeed / 2;
            LineDuty[3] = LineSpeed;
          }else{
            LineDuty[0] = LineSpeed;
            LineDuty[1] = LineSpeed;
            LineDuty[2] = LineSpeed;
            LineDuty[3] = LineSpeed;
          }
        }else if(AllLineSensorD + LineSensorE[11] + LineSensorE[12] + LineSensorE[13] > 0){
          if(OpponentGoal.distance <= 200){
            //相手のゴール際で事故らないようにするため
            LineDuty[0] = -LineSpeed;
            LineDuty[1] = LineSpeed / 2;
            LineDuty[2] = -LineSpeed;
            LineDuty[3] = LineSpeed / 2;
          }else if(MyGoal.distance <= 200){
            LineDuty[0] = -LineSpeed / 2;
            LineDuty[1] = LineSpeed;
            LineDuty[2] = -LineSpeed / 2;
            LineDuty[3] = LineSpeed;
          }else{
            LineDuty[0] = -LineSpeed;
            LineDuty[1] = (int)(LineSpeed / 1.5);
            LineDuty[2] = -LineSpeed;
            LineDuty[3] = (int)(LineSpeed / 1.5);
          }
        }
      }
      MotorDuty[0] = LineDuty[0];
      MotorDuty[1] = LineDuty[1];
      MotorDuty[2] = LineDuty[2];
      MotorDuty[3] = LineDuty[3];
      UseMotorDuty();
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
        if (TurnSpeed * (AngleX - 180) / 180 > 20) {
          AngleSpeed = -20;
        } else {
          AngleSpeed = TurnSpeed * (AngleX - 180) / 180 * -1;
        }
      }else{
        if(180 < AngleX){
          AngleSpeed = 20;
        }else{
          if (TurnSpeed * (180 - AngleX) / 180 > 20) {
            AngleSpeed = 20;
          } else {
            AngleSpeed = TurnSpeed * (180 - AngleX) / 180;
          }
        }
      }
    }else{
      if(150 < AngleX && AngleX < 180){
        if (TurnSpeed * (180 - AngleX) / 180 > 20) {
          AngleSpeed = 20;
        } else {
          AngleSpeed = TurnSpeed * (180 - AngleX) / 180;
        }
      }else{
        if(AngleX >= 180){
          if (TurnSpeed * (AngleX - 180) / 180 > 20) {
            AngleSpeed = -20;
          } else {
            AngleSpeed = TurnSpeed * (AngleX - 180) / 180 * -1;
          }
        }else{
          AngleSpeed = -20;
        }
        
      } 
    }
  }else{
    if (AngleX > 180) {
      if (TurnSpeed * (360 - AngleX) / 180 > 40) {
        AngleSpeed = 40;
      } else {
        AngleSpeed = TurnSpeed * (360 - AngleX) / 180;
      }
    } else {
      if (TurnSpeed * AngleX / 180 > 40) {
        AngleSpeed = -40;
      } else {
        AngleSpeed = TurnSpeed * AngleX / 180 * -1;
      }
    }
  }
  float SinSpeed2;
  float Gensoku;
  //ボールを持っているときに左右の動きの動きをつける
  if(isMakao == true)SinSpeed2 = SinSpeed * sin(BallPreTime / 1000000.0);
  else SinSpeed2 = 0;
  //壁際に近い時は減速する
  if((0 < LeftWall && LeftWall < 100) || (0 < RightWall && RightWall < 60)) Gensoku = 0.88;
  else Gensoku = 1.0;

  MotorDuty[0] = (int)(DefaultSpeed * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleSpeed + SinSpeed2);
  MotorDuty[1] = (int)(DefaultSpeed * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleSpeed - SinSpeed2);
  MotorDuty[2] = (int)(DefaultSpeed * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleSpeed + SinSpeed2);
  MotorDuty[3] = (int)(DefaultSpeed * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleSpeed - SinSpeed2);

  UseMotorDuty();

  if (SerialWatch == "mot") {
    printf("BallAngle : %f AngleX : %f AngleSpeed : %f",BallAngle,AngleX,AngleSpeed);
    printf(" motor1 : %d m2 : %d m3 : %d m4 : %d",MotorDuty[0],MotorDuty[1],MotorDuty[2],MotorDuty[3]);
    printf(" 回転 : %d 縦 : %d 横 : %d\n",MotorDuty[0] + MotorDuty[1] - MotorDuty[2] - MotorDuty[3],MotorDuty[0] + MotorDuty[1] + MotorDuty[2] + MotorDuty[3],MotorDuty[0] - MotorDuty[1] + MotorDuty[2] - MotorDuty[3]);  //反時計が正
  }
  BallPreTime = time_us_32();
}

void NonDribbler(float angle,bool isClockWise){
  if(isClockWise == true){
    if (AngleX > 210) {
        AngleSpeed = 40;
    } else if(AngleX > 30){
        AngleSpeed = -40;
    }else{
        AngleSpeed = 40;
    }
  }else{
    if (AngleX > 330) {
       AngleSpeed = -40;
    } else if(AngleX > 150){
      
        AngleSpeed = 40;
      
    }else{
        AngleSpeed = -40;
      
    }
  }
  float SinSpeed2;
  float Gensoku;
  //ボールを持っているときに左右の動きの動きをつける
  SinSpeed2 = 0;
  //壁際に近い時は減速する
  if(LeftWall < 71 || RightWall < 55) Gensoku = 1.0;
  else Gensoku = 1.0;

  MotorDuty[0] = (int)(DefaultSpeed * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleSpeed + SinSpeed2);
  MotorDuty[1] = (int)(DefaultSpeed * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku + AngleSpeed - SinSpeed2);
  MotorDuty[2] = (int)(DefaultSpeed * cos((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleSpeed + SinSpeed2);
  MotorDuty[3] = (int)(DefaultSpeed * sin((angle * -1 + 45) * 3.1415 / 180) * Gensoku - AngleSpeed - SinSpeed2);

  UseMotorDuty();

  if (SerialWatch == "mot") {
    printf("BallAngle : %f AngleX : %f AngleSpeed : %f",BallAngle,AngleX,AngleSpeed);
    printf(" motor1 : %d m2 : %d m3 : %d m4 : %d",MotorDuty[0],MotorDuty[1],MotorDuty[2],MotorDuty[3]);
    printf(" 回転 : %d 縦 : %d 横 : %d\n",MotorDuty[0] + MotorDuty[1] - MotorDuty[2] - MotorDuty[3],MotorDuty[0] + MotorDuty[1] + MotorDuty[2] + MotorDuty[3],MotorDuty[0] - MotorDuty[1] + MotorDuty[2] - MotorDuty[3]);  //反時計が正
  }
  BallPreTime = time_us_32();
}

void Makao(bool isClockWise,int TargetAngle){
  Brake();
  sleep_ms(2000);
  if(isClockWise == true){
    while (TargetAngle - 150 < AngleX && AngleX <= TargetAngle) {
      UseLineSensor();
      UseGyroSensor();
      //if((int)((AngleX - (TargetAngle - 140)) * 1.4) > 255){
        MainMotorState(1, 1, 160);
        MainMotorState(2, 1, 160);
        MainMotorState(3, 1, 200);
        MainMotorState(4, 1, 200);
      /*}else if((int)((AngleX - (TargetAngle - 140))* 1.4) > MakaoLeastSpeed){
        MainMotorState(1, 0, (int)((AngleX - (TargetAngle - 140)) * 1.4));
        MainMotorState(2, 0, (int)((AngleX - (TargetAngle - 140)) * 1.4));
        MainMotorState(3, 1, (int)((AngleX - (TargetAngle - 140)) * 1.4));
        MainMotorState(4, 1, (int)((AngleX - (TargetAngle - 140)) * 1.4));
      }else{
        MainMotorState(1, 0, MakaoLeastSpeed);
        MainMotorState(2, 0, MakaoLeastSpeed);
        MainMotorState(3, 1, MakaoLeastSpeed);
        MainMotorState(4, 1, MakaoLeastSpeed);
      }*/
      if(AngleX > TargetAngle - 10){
        //DribblerMotorState(0,50);
        //BLDCState(1000);
      }

      /*if(AllLineSensor > ErorrLineSensor){
        isBreak = true;
        break;
      }*/
    }
    if(isBreak == false){
      Brake();
      sleep_ms(100);
      while(10 < AngleX && AngleX < 350){
        UseGyroSensor();
        if(AngleX > 180){
          MainMotorState(1,0,LeastTurnSpeed);
          MainMotorState(2,0,LeastTurnSpeed);
          MainMotorState(3,1,LeastTurnSpeed);
          MainMotorState(4,1,LeastTurnSpeed);
        }else{
          MainMotorState(1,1,LeastTurnSpeed);
          MainMotorState(2,1,LeastTurnSpeed);
          MainMotorState(3,0,LeastTurnSpeed);
          MainMotorState(4,0,LeastTurnSpeed);
        }
      }
    }
  }else{
    while (TargetAngle <= AngleX && AngleX < TargetAngle + 150) {
      UseLineSensor();
      UseGyroSensor();
      //if((int)((TargetAngle + 140 - AngleX) * 1.4) > 255){
        MainMotorState(1, 1, 200);
        MainMotorState(2, 1, 200);
        MainMotorState(3, 1, 160);
        MainMotorState(4, 1, 160);
      /*}else if((int)((TargetAngle + 140 - AngleX) * 1.4) > MakaoLeastSpeed){
        MainMotorState(1, 1, (int)((TargetAngle + 140 - AngleX) * 1.4));
        MainMotorState(2, 1, (int)((TargetAngle + 140 - AngleX) * 1.4));
        MainMotorState(4, 0, (int)((TargetAngle + 140 - AngleX) * 1.4));
        MainMotorState(3, 0, (int)((TargetAngle + 140 - AngleX) * 1.4));
      }else{
        MainMotorState(1, 1, MakaoLeastSpeed);
        MainMotorState(2, 1, MakaoLeastSpeed);
        MainMotorState(4, 0, MakaoLeastSpeed);
        MainMotorState(3, 0, MakaoLeastSpeed);
      }*/

      if(AngleX < TargetAngle + 10){
        //DribblerMotorState(0,50);
        //BLDCState(1000);
      }

      /*if(AllLineSensor > ErorrLineSensor){
        isBreak = true;
        break;
      }*/
    }
    if(isBreak == false){
      Brake();
      sleep_ms(100);
      while(10 < AngleX && AngleX < 350){
        UseGyroSensor();
        if(AngleX > 180){
          MainMotorState(1,0,LeastTurnSpeed);
          MainMotorState(2,0,LeastTurnSpeed);
          MainMotorState(3,1,LeastTurnSpeed);
          MainMotorState(4,1,LeastTurnSpeed);
        }else{
          MainMotorState(1,1,LeastTurnSpeed);
          MainMotorState(2,1,LeastTurnSpeed);
          MainMotorState(3,0,LeastTurnSpeed);
          MainMotorState(4,0,LeastTurnSpeed);
        }
      }
    }
  }
  BLDCState(1750);
  //DribblerMotorState(0,DefaultDribblerSpeed);
  if(mode == 9 || mode == 10){
    mode -= 6;
    DefenceStart();
  }
}
