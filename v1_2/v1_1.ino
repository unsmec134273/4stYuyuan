#include <Arduino.h>
#include "line.h"
#include "motor.h"
#include "remote.h"
#include "sensor.h"
#include "env_val.h"
// #include "HMC5883L.h"
#include <Wire.h>
#include <EEPROM.h>
#include "debug.h"

#define __left 1   //读取按键，比赛方位
#define __right 0

/*******************各类flag*************/
bool remote_flag=1;   //标志遥控状态 1为需要遥控，0为停止遥控
bool car_pos_flag;
bool xunxian_flag; //巡线flag 1为黑线 0为白色

/*********************全局变量声明*************/
long int run_counter; //程序运行次数计数器
int eeaddress;
float HeadingInit;
float ScreenData[9];

void turn_to_degree(float);
void turn_to_absolute_degree(float target_degree);
void turn_to_absolute_degree_fast(float target_degree);
float offset_headingdegrees();
int beacon();
void climbing_right();
void climbing_left();
bool find_if_beacon_closed();
void screen_setup();
int find_if_beacon_on();
float tmp; //调试变量

void xunji_clear();
void xunji_calib_direction();//开始循迹前，校准至正对线
void xunji_run();

bool env_val;    //切换赛道！！！

/***************** Setup()******************/
void setup() {
        _seriaL.begin(115200);
        pinMode(envSwitch,INPUT_PULLUP);
        env_val = digitalRead(envSwitch);
        delay(5);
        while(env_val != digitalRead(envSwitch)){
          env_val = digitalRead(envSwitch);
          delay(5); //防抖
        }
        delay(5);
        while(env_val != digitalRead(envSwitch)){
          env_val = digitalRead(envSwitch);
          delay(5); //防抖
        }
        if(env_val)_seriaL.println("#######LEFT#######");
        else _seriaL.println("#######RIGHT#######");

        EEPROM.get(eeaddress,run_counter); //从EEPROM获取计数信息
        eeaddress += sizeof(long int);

        sensor_setup();

        motor_setup();
         remote_setup();
        run_counter ++;
        EEPROM.put(0,run_counter); //初始化完成，运行计数器加一
        Tone(300,400,1);
        HeadingInit = headingdegrees();
}

/*****************loop()*****************/
void loop() {
// find_if_beacon_on();
// _seriaL.println(offset_headingdegrees());
// _seriaL.print(analogRead(huidu_f));
// _seriaL.println(find_if_beacon_on_3());
// _seriaL.println(analogRead(huidu_b));

/*==================================
====================================
==============以下是正式代码==========
====================================
====================================*/
       remote_check();
       while(remote_flag){
         remote_work();
       }
       Tone(500,50,3);
       beacon();
       Tone(500,50,3);
       if(env_val == __left)
         climbing_left();
       else if(env_val == __right)
         climbing_right();
         drop();
       Tone(400,40,8);
       while(1){}


}
///////////////////////// END ///////////////////////////////////
void time_test(){
  long int begin_time = millis();
  long int err_time;
  Ultrasound_f();
  err_time = millis() - begin_time;
  _seriaL.print("Ultrasound time");
  _seriaL.print("\t");
  _seriaL.print(err_time);
  _seriaL.print("\t");
  begin_time = millis();
//  find_if_beacon_off_2();
  err_time = millis() - begin_time;
  _seriaL.print("BeaconDetect time");
  _seriaL.print("\t");
  _seriaL.println(err_time);
  delay(100);



}
