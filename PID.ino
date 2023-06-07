#include <SoftwareSerial.h>
#include <PID_v1.h>
#include <Servo.h>

SoftwareSerial mySerial(0, 1); // RX, TX pins for SoftwareSerial

double setpointX, inputX, outputX; //中心點；現在讀取的紅心座標；PID輸出
double setpointY, inputY, outputY;

bool can_fire = false;
bool is_firing = false;

unsigned long firing_start_time = 0;
unsigned long firing_current_time = 0;
const long firing_time = 150; //射擊時間


// unsigned long recoil_start_time = 0;
// unsigned long recoil_current_time = 0;
// const long recoil_time = 2 * firing_time;


Servo servoX;
Servo servoY;

const int shoot = 5; //開槍馬達腳位
const int stop = 6; //停止提升子彈馬達腳位

double Kp=2, Ki=5, Kd=1; //調PID參數
float errorX = 0, PX = 0, IX = 0, DX = 0, PID_valueX = 0;//pid直道参数
float errorY = 0, PY = 0, IY = 0, DY = 0, PID_valueY = 0;//pid直道参数
float previous_errorX = 0, previous_errorY = 0, previous_I = 0;           //误差值

// PID pidX(&inputX, &outputX, &setpointX, Kp, Ki, Kd, DIRECT);
// PID pidY(&inputY, &outputY, &setpointY, Kp, Ki, Kd, DIRECT);

void setup() {
  Serial.begin(9600); // Initialize the hardware serial port for debugging
  mySerial.begin(115200); // Initialize the software serial port

  servoX.attach(10); //馬達X腳位
  servoY.attach(9); //馬達Y腳位

  servoX.write(176);
  servoY.write(70);
  
  pinMode(shoot, OUTPUT);
  pinMode(stop, OUTPUT);

  
  // inputX = readHeartCoordinateX();
  // inputY = readHeartCoordinateY();
  

  setpointX = 400;
  setpointY = 300;
  int X_top = 205;
  int X_low = 145;
  int Y_top = 295;
  int Y_low = 235;

  //啟動PID控制器
  // pidX.SetMode(AUTOMATIC);
  // pidY.SetMode(AUTOMATIC);
}

void loop() {
  if (mySerial.available()) {
    String center = mySerial.readString();
    // Process the received data here
    // Echo back the data to the serial port
    // mySerial.write(data);
    Serial.println(center);
    int spaceindex = center.indexOf(' ');
    String strA = center.substring(0, spaceindex);
    String strB = center.substring(spaceindex + 1);

    int inputX = strA.toInt(); //讀取紅心X座標
    int inputY = strB.toInt(); //讀取紅心Y座標
    Serial.println(inputX);
    Serial.println(inputY);
    printf(inputX);
    printf(inputY);
    errorX = inputX - setpointX;
    errorY = inputY - setpointY;
    
  }
  printf("123");

  if (inputX<600 && inputX>200 && inputY<500 && inputY>100){
    can_fire = true;
    fire();
    Serial.println("can fire");
  }
  if (inputX>=500 || inputX>=300 || inputY<=400 || inputY>=200){
    can_fire = false;
  }

  // pidX.Compute();
  // pidY.Compute();
  //calc_pidX();
  //calc_pidY();
  
  PX = errorX;
  IX = IX + errorX;
  DX = errorX - previous_errorX;
  PID_valueX = (Kp * PX) + (Ki * IX) + (Kd * DX);
  previous_errorX = errorX;
  Serial.println(errorX);
  errorX=0;
  PY = errorY;
  IY = IY + errorY;
  DY = errorY - previous_errorY;
  PID_valueY = (Kp * PY) + (Ki * IY) + (Kd * DY);
  previous_errorY = errorY;
  Serial.println(errorY);
  errorY=0;
  //writeServoX();
  //writeServoY();

  int errorX = map(PID_valueX, -800, 800, 86, 266); 
  errorX = errorX + inputX;
  Serial.println(errorX);
  servoX.write(errorX);
  int errorY = map(PID_valueY, -600, 600, 10, 100); 
  errorY = errorY + inputY;
  Serial.println(errorY);
  servoY.write(errorY);

}


// double readHeartCoordinateX() {
// //接收紅心X座標
//   return ;
// }

// double readHeartCoordinateY() {
//   //接收紅心Y座標
//   return ;
// }

void calc_pidX()
{
  PX = errorX;
  IX = IX + errorX;
  DX = errorX - previous_errorX;
  PID_valueX = (Kp * PX) + (Ki * IX) + (Kd * DX);
  previous_errorX = errorX;
}

void calc_pidY()
{
  PY = errorY;
  IY = IY + errorY;
  DY = errorY - previous_errorY;
  PID_valueY = (Kp * PY) + (Ki * IY) + (Kd * DY);
  previous_errorY = errorY;
}

void writeServoX() {
  int angleX = map(PID_valueX, -800, 800, 86, 266); 
  angleX = angleX + inputX;
  servoX.write(angleX);
}

void writeServoY() {
  int angleY = map(PID_valueY, -600, 600, 10, 100); 
  angleY = angleY + inputY;
  servoY.write(angleY);
}

void fire() {

  if (can_fire && !is_firing) {
    firing_start_time = millis();
    digitalWrite(stop, HIGH);
    Serial.println("下面子彈不動");
    is_firing = true;
  }

  firing_current_time = millis();

  if (is_firing && firing_current_time - firing_start_time < firing_time) {
    digitalWrite(shoot, HIGH);
    Serial.println("fire");
  }
  else if (is_firing && firing_current_time - firing_start_time >= firing_time) {
    digitalWrite(shoot, LOW);
    Serial.println("停火");
    digitalWrite(stop, LOW);
    Serial.println("下面子彈回來");
    is_firing = false;
  }

}
