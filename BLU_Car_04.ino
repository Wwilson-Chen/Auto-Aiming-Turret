#include <SoftPWM.h>
#include <SoftPWM_timer.h>

#include <SoftwareSerial.h>

SoftwareSerial BT(3,2); //定義Arduino PIN3及PIN2分別為RX及TX腳

byte IN1=4;
byte IN2=5;
byte IN3=7;
byte IN4=6;

void setup(){
  Serial.begin(9600);
  BT.begin(38400);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop(){
  char cmd, gear;
  
  //讀取藍芽模組訊息，並傳送到電腦
  if(BT.available()){
    cmd = BT.read();
    Serial.println(cmd);
    switch(cmd){
      case 'S':
        Stop();
        break;
      case 'L':
        gear = BT.read();
        Serial.print('\t');
        Serial.println(gear);
        if(gear=='F'){
          digitalWrite(IN1, 1);
          digitalWrite(IN2, 0);
        }
        else if(gear=='B'){
          digitalWrite(IN1, 0);
          digitalWrite(IN2, 1);
        }
        else{
          digitalWrite(IN1, 0);
          digitalWrite(IN2, 0);
        }
        break;
      case 'R':
        gear = BT.read();
        Serial.print('\t');
        Serial.println(gear);
        if(gear=='F'){
          digitalWrite(IN3, 1);
          digitalWrite(IN4, 0);
        }
        else if(gear=='B'){
          digitalWrite(IN3, 0);
          digitalWrite(IN4, 1);
        }
        else{
          digitalWrite(IN3, 0);
          digitalWrite(IN4, 0);
        }
        break;
    }
  }
}

void Stop(){
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 1);
}
