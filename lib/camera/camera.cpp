#include "camera.hpp"
#include "display.hpp"
#include "../config.hpp"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <math.h>
#include <stdio.h>

int CameraDataNumber = 1;
int HowManyData = 0;
uint8_t CameraData1[] = {0,0,0,0,0,0,0};
uint8_t CameraData[] = {0,0,0,0,0,0,0};

//カメラの初期化
void CameraSetup(){
    uart_init(CameraUART,9600);
    gpio_set_function(CameraTXpin, GPIO_FUNC_UART);  // TXピン
    gpio_set_function(CameraRXpin, GPIO_FUNC_UART);  // RXピン
}

//カメラからゴールの位置と壁の位置を取得する。
void UseCamera(){
    //HowManyData = 0;
    while(uart_is_readable(CameraUART)){
        uart_read_blocking(CameraUART,CameraData1,7);
        //if(SerialWatch == "cam")printf("%u %u %u %u %u %u %u",CameraData1[0],CameraData1[1],CameraData1[2],CameraData1[3],CameraData1[4],CameraData1[5],CameraData1[6]);
    }

    //0x02が先頭識別バイト以外でこないことを想定した仕様
    for(int i = 0;i < 7;i++){
        if(CameraData1[i] == 2){
            for(int j = 0;j < 7;j++){
                if(i + j >= 7){
                    CameraData[j] = CameraData1[i + j - 7];
                }else{
                    CameraData[j] = CameraData1[i + j];
                }
            }
        }
    }
    //if(SerialWatch == "cam")printf("%u %u %u %u %u %u %u\n",CameraData[0],CameraData[1],CameraData[2],CameraData[3],CameraData[4],CameraData[5],CameraData[6]);
    if(isYellowMyGoal == 1){
        if(CameraData[1] == 255)MyGoal.X = 999;
        else MyGoal.X = 255 - (int)CameraData[1];
        if(CameraData[2] == 255)MyGoal.Y = 999;
        else MyGoal.Y = (int)CameraData[2];
        if(CameraData[3] == 255)OpponentGoal.X = 999;
        else OpponentGoal.X = 255 - (int)CameraData[3];
        if(CameraData[4] == 255)OpponentGoal.Y = 999;
        else OpponentGoal.Y = (int)CameraData[4];
    }else{
        if(CameraData[1] == 255)OpponentGoal.X = 999;
        else OpponentGoal.X = 255 - CameraData[1];
        if(CameraData[2] == 255)OpponentGoal.Y = 999;
        else OpponentGoal.Y = CameraData[2];
        if(CameraData[3] == 255)MyGoal.X = 999;
        else MyGoal.X = 255 - CameraData[3];
        if(CameraData[4] == 255)MyGoal.Y = 999;
        else MyGoal.Y = CameraData[4];
    }

    if(CameraData[5] == 255)RightWall = 999;
    else RightWall = 128 - (int)CameraData[5];
    if(CameraData[6] == 255)LeftWall = 999;
    else LeftWall = (int)CameraData[6] - 128;

    if(MyGoal.X == 999 || MyGoal.Y == 999){
        MyGoal.angle = 999.0;
        MyGoal.distance = 999.0;
    }else{
        //正面が0度で時計回りの角度に変換する。
        MyGoal.angle = atan2((128.0-MyGoal.Y),(MyGoal.X-128.0)) / 3.1415 * -180 + 90.0;
        if(MyGoal.angle < 0) MyGoal.angle += 360;
        //三平方の定理で相対距離を求める
        MyGoal.distance = sqrt((MyGoal.X-128.0)*(MyGoal.X-128.0)+(MyGoal.Y-128.0)*(MyGoal.Y-128.0));
    }
    
    if(OpponentGoal.X == 999 || OpponentGoal.Y == 999){
        OpponentGoal.angle = 999.0;
        OpponentGoal.distance = 999.0;
    }else{
        //正面が0度で時計回りの角度に変換する。
        OpponentGoal.angle = atan2((128.0-OpponentGoal.Y),(OpponentGoal.X-128.0)) / 3.1415 * -180 + 90.0;
        if(OpponentGoal.angle < 0) OpponentGoal.angle += 360;
        //三平方の定理で相対距離を求める
        OpponentGoal.distance = sqrt((OpponentGoal.X-128.0)*(OpponentGoal.X-128.0)+(OpponentGoal.Y-128.0)*(OpponentGoal.Y-128.0));
    }
    
    if(SerialWatch == "cam"){
        if(isUseDisplay){
            WriteTextOnDisplay(5,20,"          MyGoal           OppGoal",8,false,false);
            snprintf(DisplayBuffer,DisplayBufferSize,"Distance : %.1f   %.1f",MyGoal.distance,OpponentGoal.distance);
            WriteTextOnDisplay(5,30,DisplayBuffer,8,false,false);
            snprintf(DisplayBuffer,DisplayBufferSize,"Angle    : %.1f   %.1f",MyGoal.angle,OpponentGoal.angle);
            WriteTextOnDisplay(5,40,DisplayBuffer,8,false,false);
            snprintf(DisplayBuffer,DisplayBufferSize,"LWall : %.1f RWall : %.1f",LeftWall,RightWall);
            WriteTextOnDisplay(5,52,DisplayBuffer,8,false,true);
        }else{
            printf("MyGoalDis : %f Angle : %f OppGoalDis : %f Angle : %f LWall : %d RWall : %d\n"
            ,MyGoal.distance,MyGoal.angle,OpponentGoal.distance,OpponentGoal.angle,LeftWall,RightWall);
        }
    }
}