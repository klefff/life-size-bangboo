/*
SUB BOARD CODE

Code written around the following hardware:
- ESP32 WROOM (use esp dev module board on Arduino IDE)
- MG996R Servos

Code by Klef Heavy Industries 2026

THIS CODE IS PROVIDED AS-IS WITH NO WARRANTY

LICENCE AS REQUIRED
*/

#include <Arduino.h>
#include <ESP32Servo.h>

//Define hardware serial ports
#define RXD2 16
#define TXD2 17
#define SERIAL_BAUD 115200
HardwareSerial ptpSerial(2);

//Define Servos
Servo servo_a, servo_b, servo_c, servo_d;

void setup() {
  //Serial Monitor Debug
  Serial.begin(115200);

  //Attach and intialise Servos to 'home'
  servo_a.attach(15);
  servo_b.attach(2);
  servo_c.attach(26);
  servo_d.attach(27);

  // Stagger movement to avoid overcurrents
  servo_a.write(50);
  delay(20);
  servo_b.write(140);
  delay(20);
  servo_c.write(90);
  delay(20);
  servo_d.write(80);
  
  delay(100);
  servo_a.write(45);
  delay(20);
  servo_b.write(145);
  delay(20);
  servo_c.write(80);
  delay(20);
  servo_d.write(90);
  delay(100);

  //Hardware Serial
  ptpSerial.begin(SERIAL_BAUD,SERIAL_8N1,RXD2,TXD2);

  Serial.println("Peer to peer serial open");

}

//Declare Servo Positions
int a_pos = 90;
int a_pos_prev = 90;
int b_pos = 90;
int b_pos_prev = 90;
int c_pos = 90;
int c_pos_prev = 90;
int d_pos = 90;
int d_pos_prev = 90;
int mode = 0;

//Declare Prerun Positions
int i_a_pos = 0;
int i_b_pos = 0;
int i_c_pos = 0;
int i_d_pos = 0;

//Device Servo MAX and MIN
#define SER_A_MAX 160 //centre 90  //right
#define SER_A_MIN 30           // BOTTOM
#define SER_B_MAX 160 //centre 90  //left BOTTOM
#define SER_B_MIN 30
#define SER_C_MAX 95 //centre 90  //left
#define SER_C_MIN 60 //TOP
#define SER_D_MAX 110 //centre 80   //right TOP
#define SER_D_MIN 75

//Serial receive from mainboard setup
char cmd_str[40];
char sendBuff[30];
bool newData = 0;

void loop() {

//Debug
if (Serial.available()){
  int bytesRead = Serial.readBytesUntil('\n',cmd_str, sizeof(cmd_str)-1);
  cmd_str[bytesRead] = '\0';
  //Serial.println(cmd_str);

  //Declare that new data is here
  newData = 1;
  //int cmd_data = ptpSerial.parseInt();
  //while(ptpSerial.available() > 0)
  //{
  //  ptpSerial.read();
  //}
  //a_pos = cmd_data;

  //if (a_pos == 0) {
  //  a_pos = a_pos_prev;
  //}
}

//Receive data from mainboard
if (ptpSerial.available()){
  int bytesRead = ptpSerial.readBytesUntil('\n',cmd_str, sizeof(cmd_str)-1);
  cmd_str[bytesRead] = '\0';

  //Declare that new data is here
  newData = 1;
}

//If new data is received, parse it for use
if (newData){
  //Serial.println(a_pos);
  if (sscanf(cmd_str,"%d,%d,%d,%d,%d", &i_a_pos,&i_b_pos,&i_c_pos,&i_d_pos,&mode) == 5) {
    sprintf(sendBuff, "Values Read: %d, %d, %d, %d, %d",i_a_pos,i_b_pos,i_c_pos,i_d_pos,mode);
    Serial.println(sendBuff);
  }
  else {
    Serial.println(cmd_str);
   Serial.println("Value Error");
  }
  newData = 0;
}

a_pos = map(i_a_pos,0,130,SER_A_MIN,SER_A_MAX);
b_pos = map(i_b_pos,0,130,SER_B_MAX,SER_B_MIN);
c_pos = map(i_c_pos,0,35,SER_C_MAX,SER_C_MIN);
d_pos = map(i_d_pos,0,35,SER_D_MIN,SER_D_MAX);

//Check servo positions
if (a_pos > SER_A_MAX) a_pos = SER_A_MAX;
if (a_pos < SER_A_MIN) a_pos = SER_A_MIN;
if (b_pos > SER_B_MAX) b_pos = SER_B_MAX;
if (b_pos < SER_B_MIN) b_pos = SER_B_MIN;
if (c_pos > SER_C_MAX) c_pos = SER_C_MAX;
if (c_pos < SER_C_MIN) c_pos = SER_C_MIN;
if (d_pos > SER_D_MAX) d_pos = SER_D_MAX;
if (d_pos < SER_D_MIN) d_pos = SER_D_MIN;

//Move servos
if (a_pos != a_pos_prev){  //Servo A
  servo_a.write(a_pos);
}
if (b_pos != b_pos_prev){  //Servo A
  servo_b.write(b_pos);
}
if (c_pos != c_pos_prev){  //Servo A
  servo_c.write(c_pos);
}
if (d_pos != d_pos_prev){  //Servo A
  servo_d.write(d_pos);
}

//Update positions
a_pos_prev = a_pos;
b_pos_prev = b_pos;
c_pos_prev = c_pos;
d_pos_prev = d_pos;
delay(10);

}
