/*
MAINBOARD CODE

Code written around the following hardware:
- Waveshare RGB-Matrix-P2.5-96x48-F
- Waveshare ESP32-S3-RGB-Matrix

Code modified by Klef Heavy Industries 2026

THIS CODE IS PROVIDED AS-IS WITH NO WARRANTY

LICENCE AS REQUIRED
*/

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

//Define hardware serial ports
#define RXD2 45
#define TXD2 46
#define SERIAL_BAUD 115200
HardwareSerial ptpSerial(2);

//Panel parameters
#define PANEL_RES_X 96      // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 48     // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1      // Total number of panels chained one to another
 
//MatrixPanel_I2S_DMA dma_display;
MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK, myWHITE, myRED, myGREEN, myBLUE;

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {  
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

//Not used
void drawText(int colorWheelOffset)
{
  
  // draw text with a rotating colour
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  uint8_t w = 0;
  const char *str = "ESP32 DMA";
  for (w=0; w<strlen(str); w++) {
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
    dma_display->print(str[w]);
  }

  dma_display->println();
  dma_display->print(" ");
  for (w=9; w<18; w++) {
    dma_display->setTextColor(colorWheel((w*32)+colorWheelOffset));
    dma_display->print("*");
  }
  
  dma_display->println();

  dma_display->setTextColor(dma_display->color444(15,15,15));
  dma_display->println("LED MATRIX!");

  // print each letter with a fixed rainbow color
  dma_display->setTextColor(dma_display->color444(0,8,15));
  dma_display->print('3');
  dma_display->setTextColor(dma_display->color444(15,4,0));
  dma_display->print('2');
  dma_display->setTextColor(dma_display->color444(15,15,0));
  dma_display->print('x');
  dma_display->setTextColor(dma_display->color444(8,15,0));
  dma_display->print('6');
  dma_display->setTextColor(dma_display->color444(8,0,15));
  dma_display->print('4');

  // Jump a half character
  dma_display->setCursor(34, 24);
  dma_display->setTextColor(dma_display->color444(0,15,15));
  dma_display->print("*");
  dma_display->setTextColor(dma_display->color444(15,0,0));
  dma_display->print('R');
  dma_display->setTextColor(dma_display->color444(0,15,0));
  dma_display->print('G');
  dma_display->setTextColor(dma_display->color444(0,0,15));
  dma_display->print("B");
  dma_display->setTextColor(dma_display->color444(15,0,8));
  dma_display->println("*");

}


void setup() {
  //Initialise serial
  Serial.begin(115200);
  
  //Hardware Serial
  ptpSerial.begin(SERIAL_BAUD,SERIAL_8N1,RXD2,TXD2);
  Serial.println("Peer to peer serial open");

  // Module configuration
  HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   // module width
    PANEL_RES_Y,   // module height
    PANEL_CHAIN    // Chain length
  );

  mxconfig.gpio.e = 9;
  mxconfig.clkphase = false;
  mxconfig.driver = HUB75_I2S_CFG::SHIFTREG;

  // Display Setup
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setLatBlanking(1);
  dma_display->setBrightness8(120); //0-255
  dma_display->clearScreen();

  myBLACK = dma_display->color565(0, 0, 0);
  myWHITE = dma_display->color565(255, 255, 255);
  myRED = dma_display->color565(255, 0, 0);
  myGREEN = dma_display->color565(0, 255, 0);
  myBLUE = dma_display->color565(0, 0, 255);

  //Screen test images
  dma_display->fillScreen(myWHITE);
  
  // fix the screen with green
  dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(4, 15, 0));
  delay(500);

  // draw a box in yellow
  dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
  delay(500);

  // draw an 'X' in red
  dma_display->drawLine(0, 0, dma_display->width()-1, dma_display->height()-1, dma_display->color444(15, 0, 0));
  dma_display->drawLine(dma_display->width()-1, 0, 0, dma_display->height()-1, dma_display->color444(15, 0, 0));
  delay(500);

  // draw a blue circle
  dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 15, 15));
  delay(500);

  // fill a violet circle
  dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 15, 15));
  delay(500);

  // fill the screen with 'black'
  dma_display->fillScreen(dma_display->color444(0, 0, 0));
}

//Setup Serial Monitor to receive from Host (Jetson Nano)
char cmd_str[40];
bool newData = 0;
char sendBuff[30];

//Declare running mode as default 0 (= normal mode)
/* Running Modes:
0 = normal mode
1 = wave mode (rhs)
2 = AAAA mode (bocchi, pressure test)
3 = reboot mode (run startup sequence)
4 = loading mode (loop loading)
5 = sleep mode (tuckk)
6 = look mode (TODO)
7 = random look mode (regular mode)
*/
#define MAX_MODES 7
int run_mode = 7;
int cur_mode = 7;
int prev_mode = 7;
bool is_normal_mode = true;
int normal_func[] = {7,4,5};

//Declare servo values
int a_pos = 90;
int b_pos = 90;
int c_pos = 90;
int d_pos = 90;

// Eye positions
int source_x = 0;
int source_y = 0;
int target_x = 0;
int target_y = 0;

const int eye_Gap = 50;
int eye_LeftHomeX = 0;
int eye_LeftHomeY = 20;
int eye_Size = 12;
int eye_Thickness = 5;

int eye_Width = eye_Size;

//Animation params
const int fps = 17;

uint8_t wheelval = 0;

void loop() {
  //receive from python host to pass onto the servo board
  while (Serial.available() > 0){
    //Read commands (EYE POS, 1 MODE) TODO: add servos for manual mode
    int bytesRead = Serial.readBytesUntil('\n',cmd_str,sizeof(cmd_str));
    cmd_str[bytesRead] = '\0'; //Terminate the data

    //Declare that new data is here
    newData = 1;

    //Debug
    //ptpSerial.println(cmd_str);
    Serial.println(cmd_str);
  }

  //If new data is received, process it for running modes
  if (newData){
    //Check that the data formatting is correct to:
    if (sscanf(cmd_str,"%d,%d,%d", &target_x,&target_y,&run_mode) == 3) {
      //sprintf(sendBuff, "Values Read: %d, %d, %d",eye_x,eye_y,run_mode);
      //Serial.println(run_mode);

      //Check the mode and data
      if (run_mode > MAX_MODES) {
        Serial.println("No Such Mode");
      }
      else if (run_mode == 0) {
        is_normal_mode = true;
        cur_mode = 0;
      }
      else {
        is_normal_mode = false;
        cur_mode = run_mode;
      };
    }
    else {
      Serial.println(cmd_str);
      Serial.println("Value Error");
      cur_mode = prev_mode;
    }
    newData = 0; //Reset
  }

  // If in normal mode, randomly choose between these set modes
  if (is_normal_mode){
    cur_mode = normal_func[random(0,2)];
  }

  if (cur_mode != prev_mode){
    //TODO: Run transition animation
  }

  //Run modes
  switch (cur_mode) {
    // Normal Mode (look around, 10 seconds ish)
    case 0:
      //It should never reach this case
      break;
    // Wave Mode (wave around one arm, switch back to normal mode after?)
    case 1: {
      target_x = 56;
      target_y = 20;
      drawHappyEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      source_x = target_x;
      source_y = target_y;
      ptpSerial.println("120,10,30,30,1");
      delay(600);
      ptpSerial.println("100,10,30,30,1");
      delay(100);
      ptpSerial.println("120,10,30,30,1");
      delay(100);
      ptpSerial.println("100,10,30,30,1");
      delay(100);
      ptpSerial.println("120,10,30,30,1");
      delay(100);
      ptpSerial.println("100,10,30,30,1");
      delay(100);
      ptpSerial.println("120,10,30,30,1");
      delay(100);
      ptpSerial.println("100,10,30,30,1");
      delay(100);
      ptpSerial.println("120,10,30,30,1");
      delay(100);
      ptpSerial.println("10,10,15,15,1");
      //drawHappyEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      //delay(1000);
      cur_mode = 7;
    } break;
    // AAAAAAAA Mode 
    case 2: {
      //Need to add a timer (millis?) max 5 seconds maybe
      /*
      unsigned long ct_timer = millis();
      while(millis()-ct_timer < 5000)
      {
        long a_ran_cmd = random(40,175);
        long b_ran_cmd = random(40,175);
        long c_ran_cmd = random(40,175);
        long d_ran_cmd = random(40,175);
        sprintf(sendBuff, "%ld, %ld, %ld, %ld, 1",a_ran_cmd,b_ran_cmd,c_ran_cmd,d_ran_cmd);
        ptpSerial.println(sendBuff);
        delay(24); //TODO: this needs to be FPS
        //Serial.println(sendBuff);
      
      }
      */
      for (int i = 0; i < 100; i++){
        target_x = random(46,60);
        target_y = random(10,5);
        if (target_x != source_x || target_y != source_y){
          drawSunEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        }
        source_x = target_x;
        source_y = target_y;
      }
      cur_mode = 7;
    } break;
    // TODO: Reboot Mode (reboot sequence that should appear at the setup command)
    case 3: {}
    //Should be just an animated gif mode
      break;
    // Look Mode 
    case 4: {
      target_x = random(40,60);
      target_y = random(10,40);
      if (target_x != source_x || target_y != source_y){
        drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      }
      source_x = target_x;
      source_y = target_y;
      long delayTime = random(500,2500);
      if(delayTime > 1500){
        delay(delayTime/2);
        drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        delay(100);
        drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        if (random(1,3) == 1){
          delay(100);
          drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
          delay(100);
          drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        }
        delay(delayTime/2);
      }
      else {
        delay(delayTime);
      }
    }
    //animated gif
      break;
    //Sleep mode (sleep animation)
    case 5: {
      target_x = 48;
      target_y = 20;
      drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      source_x = target_x;
      source_y = target_y;
      delay(1000);
      drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      delay(100);
      drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      delay(100);
      drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      delay(100);
      drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      delay(1000);
      drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      delay(100);
      for (int i = 0; i < 3; i++){
        target_x = 48;
        target_y = 30;
        drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        source_y = target_y;
        delay(400);
        target_y = 20;
        drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        source_y = target_y;
        delay(400);
      }
      target_x = 30;
      target_y = 25;
      drawFastEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);      
      source_x = target_x;
      source_y = target_y;
      target_x = 56;
      target_y = 25;
      drawFastEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);      
      source_x = target_x;
      source_y = target_y;      
      target_x = 30;
      target_y = 25;
      drawFastEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);      
      source_x = target_x;
      source_y = target_y;

      cur_mode = 7;
    }
    //animated gif mode
      break;
    // Manual Look around mode
    case 6: {    
      if (target_x != source_x || target_y != source_y){
        drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      }
      source_x = target_x;
      source_y = target_y;
    }
    //base ear position around eyes
      break;
    // Auto look around (10 sec interval)
    case 7: {
      target_x = random(40,60);
      target_y = random(10,40);
      if (target_x != source_x || target_y != source_y){
        drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
      }
      source_x = target_x;
      source_y = target_y;
      long delayTime = random(500,2500);

      if (random(1,15) == 3){
        cur_mode = 5;
      }

      if(delayTime > 1500){
        delay(delayTime/2);
        drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        delay(100);
        drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        if (random(1,3) == 1){
          delay(100);
          drawSleepEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
          delay(100);
          drawEyes(source_x, source_y, target_x, target_y, eye_Size*2, 1);
        }
        delay(delayTime/2);
      }
      else {
        delay(delayTime);
      }
    }
    //We have the base already
      break;
    default: {}
      break;
  }

  prev_mode = cur_mode;
}

//Bezier Animation Driver
float bezierOut(float a_1, float a_2, float time){
  float P_0_x = -0.5;
  float P_0_y = 0.5;
  float P_1_x = 0.0;
  float P_1_y = 0.0;
  float P_2_x = 1.0;
  float P_2_y = 1.0;
  float P_3_x = 1.5;
  float P_3_y = 0.5;
  
  float d_3_x = pow(abs(P_3_x-P_2_x), a_1);
  float d_2_x = pow(abs(P_2_x-P_1_x), a_1);
  float d_1_x = pow(abs(P_1_x-P_0_x), a_1);

  float d_3_y = pow(abs(P_3_y-P_2_y), a_1);
  float d_2_y = pow(abs(P_2_y-P_1_y), a_1);
  float d_1_y = pow(abs(P_1_y-P_0_y), a_1);

  float T_2_x = (pow(d_3_x,2)*P_1_x-pow(d_2_x,2)*P_3_x+(2*pow(d_3_x,2)+3*d_3_x*d_2_x+pow(d_2_x,2))*P_2_x)/(3*d_3_x*(d_3_x+d_2_x));
  float T_2_y = (pow(d_3_y,2)*P_1_y-pow(d_2_y,2)*P_3_y+(2*pow(d_3_y,2)+3*d_3_y*d_2_y+pow(d_2_y,2))*P_2_y)/(3*d_3_y*(d_3_y+d_2_y));

  float T_1_x = (pow(d_1_x,2)*P_2_x-pow(d_2_x,2)*P_0_x+(2*pow(d_1_x,2)+3*d_1_x*d_2_x+pow(d_2_x,2))*P_1_x)/(3*d_1_x*(d_1_x+d_2_x));
  float T_1_y = (pow(d_1_y,2)*P_2_y-pow(d_2_y,2)*P_0_y+(2*pow(d_1_y,2)+3*d_1_y*d_2_y+pow(d_2_y,2))*P_1_y)/(3*d_1_y*(d_1_y+d_2_y));

  float B_x = P_1_x*pow((1-time),3)+T_1_x*3*pow((1-time),2)*time+T_2_x*3*(1-time)*pow(time,2)+P_2_x*pow(time,3);
  float B_y = P_1_y*pow((1-time),3)+T_1_y*3*pow((1-time),2)*time+T_2_y*3*(1-time)*pow(time,2)+P_2_y*pow(time,3);

  return B_y;
}

// Oval drawing for smear frames
void drawOval(int x, int y, int angle, int width, int maj_radius, int min_radius, int o_thickness, int o_R, int o_G, int o_B){
  int min_thickness = min_radius-o_thickness;

  if(min_thickness < 0){
    min_thickness = 0;
  } 
 
  float M_1_x = x - (cos(angle*M_PI/180.0+M_PI_2)*maj_radius);
  float M_1_y = y - (sin(angle*M_PI/180.0+M_PI_2)*maj_radius);
  float M_2_x = x - (cos(angle*M_PI/180.0+M_PI+M_PI_2)*maj_radius);
  float M_2_y = y - (sin(angle*M_PI/180.0+M_PI+M_PI_2)*maj_radius);

  float M_1_x_i = x - (cos(angle*M_PI/180.0+M_PI_2)*(maj_radius-o_thickness));
  float M_1_y_i = y - (sin(angle*M_PI/180.0+M_PI_2)*(maj_radius-o_thickness));
  float M_2_x_i = x - (cos(angle*M_PI/180.0+M_PI+M_PI_2)*(maj_radius-o_thickness));
  float M_2_y_i = y - (sin(angle*M_PI/180.0+M_PI+M_PI_2)*(maj_radius-o_thickness));

  float S_x = x - cos(angle*M_PI/180.0)*width;
  float S_y = y - sin(angle*M_PI/180.0)*width;

  float S_1_x = S_x - (cos(angle*M_PI/180.0+M_PI_2)*min_radius);
  float S_1_y = S_y - (sin(angle*M_PI/180.0+M_PI_2)*min_radius);
  float S_2_x = S_x - (cos(angle*M_PI/180.0+M_PI+M_PI_2)*min_radius);
  float S_2_y = S_y - (sin(angle*M_PI/180.0+M_PI+M_PI_2)*min_radius);

  float S_1_x_i = S_x - (cos(angle*M_PI/180.0+M_PI_2)*min_thickness);
  float S_1_y_i = S_y - (sin(angle*M_PI/180.0+M_PI_2)*min_thickness);
  float S_2_x_i = S_x - (cos(angle*M_PI/180.0+M_PI+M_PI_2)*min_thickness);
  float S_2_y_i = S_y - (sin(angle*M_PI/180.0+M_PI+M_PI_2)*min_thickness);



  //dma_display->clearScreen();

  dma_display->fillCircle(int(x), int(y), maj_radius, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(int(S_x), int(S_y), min_radius, dma_display->color565(150, 255, 84));
  dma_display->fillTriangle(M_1_x, M_1_y, S_1_x, S_1_y, S_2_x, S_2_y, dma_display->color565(150, 255, 84));
  dma_display->fillTriangle(M_1_x, M_1_y, S_2_x, S_2_y, M_2_x, M_2_y, dma_display->color565(150, 255, 84));
  
  dma_display->fillCircle(int(x), int(y), maj_radius-o_thickness, dma_display->color565(0, 0, 0));
  dma_display->fillCircle(int(S_x), int(S_y), min_thickness, dma_display->color565(0, 0, 0));

  //dma_display->fillCircle(int(M_1_x), int(M_1_y), 1, dma_display->color565(225, 100, 23));
  //dma_display->fillCircle(int(M_2_x), int(M_2_y), 1, dma_display->color565(100, 246, 23));

  //dma_display->fillCircle(int(S_1_x), int(S_1_y), 1, dma_display->color565(225, 100, 23));
  //dma_display->fillCircle(int(S_2_x), int(S_2_y), 1, dma_display->color565(100, 246, 23));


  dma_display->fillTriangle(M_1_x_i, M_1_y_i, S_1_x_i, S_1_y_i, S_2_x_i, S_2_y_i, dma_display->color565(0, 0, 0));
  dma_display->fillTriangle(M_1_x_i, M_1_y_i, S_2_x_i, S_2_y_i, M_2_x_i, M_2_y_i, dma_display->color565(0, 0, 0));

}

// Draw eyes based on position and size requirements
void drawEyes(int or_x, int or_y, int t_x, int t_y, int width, int mode){
  //Servo Parameters
  char earBuff[30];

  //Offset Calculator
  int of_str_y = 5;
  int of_str_w = 10;

  //Get Next Position Distance
  int x_dist = t_x-or_x;
  int y_dist = t_y-or_y;
  
  //Calculate the angle of travel 
  double a_angle =  atan(1.0*(t_y-or_y)/(t_x-or_x));

  //Update the angle based on polar coordinates
  if (x_dist > 0){
    a_angle = a_angle;
  }
  else if (x_dist < 0 && y_dist >= 0){
    a_angle = a_angle+M_PI;
  }
  else if (x_dist < 0 && y_dist < 0){
    a_angle = a_angle-M_PI;
  }
  else if (x_dist == 0 && y_dist > 0){
    a_angle = M_PI_2;
  }
  else if (x_dist == 0 && y_dist < 0){
    a_angle = -1.0*M_PI_2;
  }
  else {
    a_angle = 0;
  }

  //Update angle to degrees (cus im dumb)
  a_angle = a_angle*180/M_PI;

  //Parameters

  //Travel time TODO: change based on distance
  int slewTime = 250;

  if (abs(x_dist)+abs(y_dist) < 10){ //reduce travel time if eyes not need to move as far
    slewTime = 100;
  }

  int intervalCount = slewTime*1.0/fps + 1;

  //TODO: reduce width of eyes based on position on screen
  int updated_width = width;

  //offset left and right Y based on eye position
  int off_y_str = 20;
  int updated_y_next_l = 0;
  int updated_y_next_r = 0;

  //get final updated position to outside
  if (t_y > 24){
    updated_y_next_l = t_y + int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y - int(off_y_str*(t_x-48)/48.0);
  }
  else if (t_y < 24){
    updated_y_next_l = t_y - int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y + int(off_y_str*(t_x-48)/48.0);
  }
  else {
    updated_y_next_l = t_y;
    updated_y_next_r = t_y;
  }

  //Motion blur strength
  float m_Str = 1.5;

  //Positioning declarations
  int x_prev = or_x;
  int y_prev = or_y;

  dma_display->clearScreen();
  
  //Motion engine
  if (abs(x_dist) + abs(y_dist) > 5){ //don't perform motion if distance is small enough
    
    //Perform motion over the requested time frame
    for (int i = 0; i<intervalCount;i++){

      //Generate the bezier curve for that time section
      int x_next = x_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_x;
      int y_next = y_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_y;
      dma_display->clearScreen();

      //TODO: determine width positioning
      updated_width = width; //- int(20*abs(x_next-48)/48.0);

      //TODO: move updated position to outside

        if (y_next > 24){
          updated_y_next_l = y_next + int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next - int(off_y_str*(x_next-48)/48.0);
        }
        else if (y_next < 24){
          updated_y_next_l = y_next - int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next + int(off_y_str*(x_next-48)/48.0);
        }
        else {
          updated_y_next_l = y_next;
          updated_y_next_r = y_next;
        }

      drawOval(x_next-updated_width,updated_y_next_l,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      drawOval(x_next+updated_width,updated_y_next_r,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);

      //dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing

      //dma_display->print(x_pos);
      x_prev = x_next;
      y_prev = y_next;
      delay(fps);
      dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
      dma_display->print(updated_width);

      //Command Motors
      sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(x_next,0,94,-20,55)-map(y_next,0,48,-20,30),map(x_next,0,94,55,-20)-map(y_next,0,48,-20,30));
      ptpSerial.println(earBuff);


    } 
  }
  dma_display->clearScreen();

  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  dma_display->print(updated_width);
    //Command Motors
  sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(t_x,0,94,-20,55)-map(t_y,0,48,-20,30),map(t_x,0,94,55,-20)-map(t_y,0,48,-20,30));
  ptpSerial.println(earBuff);
}

// drawEyes but sleepy
void drawSleepEyes(int or_x, int or_y, int t_x, int t_y, int width, int mode){ 
  //Servo Parameters
  char earBuff[30];


  //Offset Calculator
  int of_str_y = 5;
  int of_str_w = 10;

  //Get Next Position Distance
  int x_dist = t_x-or_x;
  int y_dist = t_y-or_y;
  
  //Calculate the angle of travel 
  double a_angle =  atan(1.0*(t_y-or_y)/(t_x-or_x));

  //Update the angle based on polar coordinates
  if (x_dist > 0){
    a_angle = a_angle;
  }
  else if (x_dist < 0 && y_dist >= 0){
    a_angle = a_angle+M_PI;
  }
  else if (x_dist < 0 && y_dist < 0){
    a_angle = a_angle-M_PI;
  }
  else if (x_dist == 0 && y_dist > 0){
    a_angle = M_PI_2;
  }
  else if (x_dist == 0 && y_dist < 0){
    a_angle = -1.0*M_PI_2;
  }
  else {
    a_angle = 0;
  }

  //Update angle to degrees (cus im dumb)
  a_angle = a_angle*180/M_PI;

  //Parameters

  //Travel time TODO: change based on distance
  int slewTime = 2000;

  if (abs(x_dist)+abs(y_dist) < 10){ //reduce travel time if eyes not need to move as far
    slewTime = 100;
  }

  int intervalCount = slewTime*1.0/fps + 1;

  //TODO: reduce width of eyes based on position on screen
  int updated_width = width;

  //offset left and right Y based on eye position
  int off_y_str = 20;
  int updated_y_next_l = 0;
  int updated_y_next_r = 0;

  //get final updated position to outside
  if (t_y > 24){
    updated_y_next_l = t_y + int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y - int(off_y_str*(t_x-48)/48.0);
  }
  else if (t_y < 24){
    updated_y_next_l = t_y - int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y + int(off_y_str*(t_x-48)/48.0);
  }
  else {
    updated_y_next_l = t_y;
    updated_y_next_r = t_y;
  }

  //Motion blur strength
  float m_Str = 1.5;

  //Positioning declarations
  int x_prev = or_x;
  int y_prev = or_y;

  dma_display->clearScreen();
  
  //Motion engine
  if (abs(x_dist) + abs(y_dist) > 0){ //don't perform motion if distance is small enough
    
    //Perform motion over the requested time frame
    for (int i = 0; i<intervalCount;i++){

      //Generate the bezier curve for that time section
      int x_next = x_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_x;
      int y_next = y_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_y;
      dma_display->clearScreen();

      //TODO: determine width positioning
      updated_width = width; //- int(20*abs(x_next-48)/48.0);

      //TODO: move updated position to outside

        if (y_next > 24){
          updated_y_next_l = y_next + int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next - int(off_y_str*(x_next-48)/48.0);
        }
        else if (y_next < 24){
          updated_y_next_l = y_next - int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next + int(off_y_str*(x_next-48)/48.0);
        }
        else {
          updated_y_next_l = y_next;
          updated_y_next_r = y_next;
        }

      //drawOval(x_next-updated_width,updated_y_next_l,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      //drawOval(x_next+updated_width,updated_y_next_r,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      dma_display->fillRect(x_next-updated_width-eye_Size,updated_y_next_l,width,5,dma_display->color565(150, 255, 84));
      dma_display->fillRect(x_next+updated_width-eye_Size,updated_y_next_r,width,5,dma_display->color565(150, 255, 84));

      //dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing

      //dma_display->print(x_pos);
      x_prev = x_next;
      y_prev = y_next;
      delay(fps);
      dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
      dma_display->print(updated_width);

      //Command Motors
      sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(x_next,0,94,-20,55)-map(y_next,0,48,-20,30),map(x_next,0,94,55,-20)-map(y_next,0,48,-20,30));
      ptpSerial.println(earBuff);


    } 
  }
  dma_display->clearScreen();

 // dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size, dma_display->color565(150, 255, 84));
  //dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  //dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size, dma_display->color565(150, 255, 84));
  //dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
    dma_display->fillRect(t_x-updated_width-eye_Size,updated_y_next_l,width,5,dma_display->color565(150, 255, 84));
    dma_display->fillRect(t_x+updated_width-eye_Size,updated_y_next_r,width,5,dma_display->color565(150, 255, 84));
  
  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  dma_display->print(updated_width);
    //Command Motors
  sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(t_x,0,94,-20,55)-map(t_y,0,48,-20,30),map(t_x,0,94,55,-20)-map(t_y,0,48,-20,30));
  ptpSerial.println(earBuff);
}

// drawEyes but happy
void drawHappyEyes(int or_x, int or_y, int t_x, int t_y, int width, int mode){
   
  //Servo Parameters
  char earBuff[30];


  //Offset Calculator
  int of_str_y = 5;
  int of_str_w = 10;

  //Get Next Position Distance
  int x_dist = t_x-or_x;
  int y_dist = t_y-or_y;
  
  //Calculate the angle of travel 
  double a_angle =  atan(1.0*(t_y-or_y)/(t_x-or_x));

  //Update the angle based on polar coordinates
  if (x_dist > 0){
    a_angle = a_angle;
  }
  else if (x_dist < 0 && y_dist >= 0){
    a_angle = a_angle+M_PI;
  }
  else if (x_dist < 0 && y_dist < 0){
    a_angle = a_angle-M_PI;
  }
  else if (x_dist == 0 && y_dist > 0){
    a_angle = M_PI_2;
  }
  else if (x_dist == 0 && y_dist < 0){
    a_angle = -1.0*M_PI_2;
  }
  else {
    a_angle = 0;
  }

  //Update angle to degrees (cus im dumb)
  a_angle = a_angle*180/M_PI;

  //Parameters

  //Travel time TODO: change based on distance
  int slewTime = 150;

  if (abs(x_dist)+abs(y_dist) < 10){ //reduce travel time if eyes not need to move as far
    slewTime = 100;
  }

  int intervalCount = slewTime*1.0/fps + 1;

  //TODO: reduce width of eyes based on position on screen
  int updated_width = width;

  //offset left and right Y based on eye position
  int off_y_str = 20;
  int updated_y_next_l = 0;
  int updated_y_next_r = 0;
  int large_y_l = 0;
  int large_y_r = 0;

  //get final updated position to outside
  if (t_y > 24){
    updated_y_next_l = t_y + int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y - int(off_y_str*(t_x-48)/48.0);
  }
  else if (t_y < 24){
    updated_y_next_l = t_y - int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y + int(off_y_str*(t_x-48)/48.0);
  }
  else {
    updated_y_next_l = t_y;
    updated_y_next_r = t_y;
  }

  //Motion blur strength
  float m_Str = 1.5;

  //Positioning declarations
  int x_prev = or_x;
  int y_prev = or_y;

  dma_display->clearScreen();
  
  //Motion engine
  if (abs(x_dist) + abs(y_dist) > 5){ //don't perform motion if distance is small enough
    
    //Perform motion over the requested time frame
    for (int i = 0; i<intervalCount;i++){

      //Generate the bezier curve for that time section
      int x_next = x_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_x;
      int y_next = y_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_y;
      dma_display->clearScreen();

      //TODO: determine width positioning
      updated_width = width; //- int(20*abs(x_next-48)/48.0);

      //TODO: move updated position to outside

        if (y_next > 24){
          updated_y_next_l = y_next + int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next - int(off_y_str*(x_next-48)/48.0);
          large_y_l = y_next + 3*int(off_y_str*(x_next-48)/48.0);
          large_y_r = y_next - 3*int(off_y_str*(x_next-48)/48.0);
        }
        else if (y_next < 24){
          updated_y_next_l = y_next - int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next + int(off_y_str*(x_next-48)/48.0);
          large_y_l = y_next - 3*int(off_y_str*(x_next-48)/48.0);
          large_y_r = y_next + 3*int(off_y_str*(x_next-48)/48.0);
        }
        else {
          updated_y_next_l = y_next;
          updated_y_next_r = y_next;
        }

      drawOval(x_next-updated_width,updated_y_next_l,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      drawOval(x_next+updated_width,updated_y_next_r,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      dma_display->fillTriangle(x_next-updated_width*3,large_y_l+3,x_next+updated_width*3,large_y_r+3,48,500,dma_display->color565(1, 0, 0));

      //dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing

      //dma_display->print(x_pos);
      x_prev = x_next;
      y_prev = y_next;
      delay(fps);
      dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
      dma_display->print(updated_width);

      //Command Motors
      sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(x_next,0,94,-20,55)-map(y_next,0,48,-20,30),map(x_next,0,94,55,-20)-map(y_next,0,48,-20,30));
      ptpSerial.println(earBuff);


    } 
  }
  dma_display->clearScreen();

  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->fillTriangle(t_x-updated_width*3,large_y_l+3,t_x+updated_width*3,large_y_r+3,48,500,dma_display->color565(1, 0, 0));

  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  dma_display->print(updated_width);
    //Command Motors
  sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(t_x,0,94,-20,55)-map(t_y,0,48,-20,30),map(t_x,0,94,55,-20)-map(t_y,0,48,-20,30));
  ptpSerial.println(earBuff);
}

// drawEyes but the slew rate is very fast
void drawFastEyes(int or_x, int or_y, int t_x, int t_y, int width, int mode){
   
  //Servo Parameters
  char earBuff[30];


  //Offset Calculator
  int of_str_y = 5;
  int of_str_w = 10;

  //Get Next Position Distance
  int x_dist = t_x-or_x;
  int y_dist = t_y-or_y;
  
  //Calculate the angle of travel 
  double a_angle =  atan(1.0*(t_y-or_y)/(t_x-or_x));

  //Update the angle based on polar coordinates
  if (x_dist > 0){
    a_angle = a_angle;
  }
  else if (x_dist < 0 && y_dist >= 0){
    a_angle = a_angle+M_PI;
  }
  else if (x_dist < 0 && y_dist < 0){
    a_angle = a_angle-M_PI;
  }
  else if (x_dist == 0 && y_dist > 0){
    a_angle = M_PI_2;
  }
  else if (x_dist == 0 && y_dist < 0){
    a_angle = -1.0*M_PI_2;
  }
  else {
    a_angle = 0;
  }

  //Update angle to degrees (cus im dumb)
  a_angle = a_angle*180/M_PI;

  //Parameters

  //Travel time TODO: change based on distance
  int slewTime = 50;

  if (abs(x_dist)+abs(y_dist) < 10){ //reduce travel time if eyes not need to move as far
    slewTime = 100;
  }

  int intervalCount = slewTime*1.0/fps + 1;

  //TODO: reduce width of eyes based on position on screen
  int updated_width = width;

  //offset left and right Y based on eye position
  int off_y_str = 20;
  int updated_y_next_l = 0;
  int updated_y_next_r = 0;

  //get final updated position to outside
  if (t_y > 24){
    updated_y_next_l = t_y + int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y - int(off_y_str*(t_x-48)/48.0);
  }
  else if (t_y < 24){
    updated_y_next_l = t_y - int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y + int(off_y_str*(t_x-48)/48.0);
  }
  else {
    updated_y_next_l = t_y;
    updated_y_next_r = t_y;
  }

  //Motion blur strength
  float m_Str = 1.5;

  //Positioning declarations
  int x_prev = or_x;
  int y_prev = or_y;

  dma_display->clearScreen();
  
  //Motion engine
  if (abs(x_dist) + abs(y_dist) > 5){ //don't perform motion if distance is small enough
    
    //Perform motion over the requested time frame
    for (int i = 0; i<intervalCount;i++){

      //Generate the bezier curve for that time section
      int x_next = x_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_x;
      int y_next = y_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_y;
      dma_display->clearScreen();

      //TODO: determine width positioning
      updated_width = width; //- int(20*abs(x_next-48)/48.0);

      //TODO: move updated position to outside

        if (y_next > 24){
          updated_y_next_l = y_next + int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next - int(off_y_str*(x_next-48)/48.0);
        }
        else if (y_next < 24){
          updated_y_next_l = y_next - int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next + int(off_y_str*(x_next-48)/48.0);
        }
        else {
          updated_y_next_l = y_next;
          updated_y_next_r = y_next;
        }

      drawOval(x_next-updated_width,updated_y_next_l,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      drawOval(x_next+updated_width,updated_y_next_r,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);

      //dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing

      //dma_display->print(x_pos);
      x_prev = x_next;
      y_prev = y_next;
      delay(fps);
      dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
      dma_display->print(updated_width);

      //Command Motors
      sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(x_next,0,94,-20,55)-map(y_next,0,48,-20,30),map(x_next,0,94,55,-20)-map(y_next,0,48,-20,30));
      ptpSerial.println(earBuff);


    } 
  }
  dma_display->clearScreen();

  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  dma_display->print(updated_width);
    //Command Motors
  sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",10,10,map(t_x,0,94,-20,55)-map(t_y,0,48,-20,30),map(t_x,0,94,55,-20)-map(t_y,0,48,-20,30));
  ptpSerial.println(earBuff);
}

// drawEyes but the slew rate is REALLY fast
void drawSunEyes(int or_x, int or_y, int t_x, int t_y, int width, int mode){
   
  //Servo Parameters
  char earBuff[30];


  //Offset Calculator
  int of_str_y = 5;
  int of_str_w = 10;

  //Get Next Position Distance
  int x_dist = t_x-or_x;
  int y_dist = t_y-or_y;
  
  //Calculate the angle of travel 
  double a_angle =  atan(1.0*(t_y-or_y)/(t_x-or_x));

  //Update the angle based on polar coordinates
  if (x_dist > 0){
    a_angle = a_angle;
  }
  else if (x_dist < 0 && y_dist >= 0){
    a_angle = a_angle+M_PI;
  }
  else if (x_dist < 0 && y_dist < 0){
    a_angle = a_angle-M_PI;
  }
  else if (x_dist == 0 && y_dist > 0){
    a_angle = M_PI_2;
  }
  else if (x_dist == 0 && y_dist < 0){
    a_angle = -1.0*M_PI_2;
  }
  else {
    a_angle = 0;
  }

  //Update angle to degrees (cus im dumb)
  a_angle = a_angle*180/M_PI;

  //Parameters

  //Travel time TODO: change based on distance
  int slewTime = 50;

  if (abs(x_dist)+abs(y_dist) < 10){ //reduce travel time if eyes not need to move as far
    slewTime = 100;
  }

  int intervalCount = slewTime*1.0/fps + 1;

  //TODO: reduce width of eyes based on position on screen
  int updated_width = width;

  //offset left and right Y based on eye position
  int off_y_str = 20;
  int updated_y_next_l = 0;
  int updated_y_next_r = 0;

  //get final updated position to outside
  if (t_y > 24){
    updated_y_next_l = t_y + int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y - int(off_y_str*(t_x-48)/48.0);
  }
  else if (t_y < 24){
    updated_y_next_l = t_y - int(off_y_str*(t_x-48)/48.0);
    updated_y_next_r = t_y + int(off_y_str*(t_x-48)/48.0);
  }
  else {
    updated_y_next_l = t_y;
    updated_y_next_r = t_y;
  }

  //Motion blur strength
  float m_Str = 1.5;

  //Positioning declarations
  int x_prev = or_x;
  int y_prev = or_y;

  dma_display->clearScreen();
  
  //Motion engine
  if (abs(x_dist) + abs(y_dist) > 5){ //don't perform motion if distance is small enough
    
    //Perform motion over the requested time frame
    for (int i = 0; i<intervalCount;i++){

      //Generate the bezier curve for that time section
      int x_next = x_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_x;
      int y_next = y_dist*bezierOut(0.5,2,(i*1.0)/intervalCount)+or_y;
      dma_display->clearScreen();

      //TODO: determine width positioning
      updated_width = width; //- int(20*abs(x_next-48)/48.0);

      //TODO: move updated position to outside

        if (y_next > 24){
          updated_y_next_l = y_next + int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next - int(off_y_str*(x_next-48)/48.0);
        }
        else if (y_next < 24){
          updated_y_next_l = y_next - int(off_y_str*(x_next-48)/48.0);
          updated_y_next_r = y_next + int(off_y_str*(x_next-48)/48.0);
        }
        else {
          updated_y_next_l = y_next;
          updated_y_next_r = y_next;
        }

      drawOval(x_next-updated_width,updated_y_next_l,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);
      drawOval(x_next+updated_width,updated_y_next_r,a_angle,abs(x_prev-x_next)*m_Str,eye_Size,12,eye_Thickness,150, 255, 84);

      //dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing

      //dma_display->print(x_pos);
      x_prev = x_next;
      y_prev = y_next;
      delay(fps);
      dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
      dma_display->print(updated_width);

      //Command Motors
      sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",120,120,map(x_next,0,94,-20,55)-map(y_next,0,48,-20,30),map(x_next,0,94,55,-20)-map(y_next,0,48,-20,30));
      ptpSerial.println(earBuff);


    } 
  }
  dma_display->clearScreen();

  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x-updated_width, updated_y_next_l, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size, dma_display->color565(150, 255, 84));
  dma_display->fillCircle(t_x+updated_width, updated_y_next_r, eye_Size-eye_Thickness, dma_display->color565(1,0,0));
  dma_display->setCursor(5, 0);    // start at top left, with 8 pixel of spacing
  dma_display->print(updated_width);
    //Command Motors
  sprintf(earBuff, "%ld, %ld, %ld, %ld, 1",120,120,map(t_x,0,94,-20,55)-map(t_y,0,48,-20,30),map(t_x,0,94,55,-20)-map(t_y,0,48,-20,30));
  ptpSerial.println(earBuff);
}