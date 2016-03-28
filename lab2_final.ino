#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include <Wire.h>
#include <LSM303.h>

#include "splash.h"
/* definitions */
#define QIBLAH 135.377639     /*qiblah direction for alex*/
#define FLICKER_INTERVAL 100  /*flicker interval when compass direction is near qiblah*/
#define CHAR_H 8              /*lcd character height (for size 1)*/
#define CHAR_W 5              /*lcd character width (for size 1)*/
#define LED_PIN 8             /*lcd flash pin*/


/*pin connections*/
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// pin 8 - LED

/*initiations*/
LSM303 compass; //lsm303 sensor
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3); //nokia display

int r = 24;  // radius of compass rose
int x0= 58;  // x-origin
int y0 = 24; // y-origin
float angleActual;  //actual angle measured by the sensor
int last_on; //for led flickering



/*function definitions*/
void drawCompass(); // drawing on lcd
void calculateCompass(); //calculate angle
void drawDirections();  //drawing n,w,e,s,q on screen
void drawSplash();    //draw splash intro

/*code*/
void setup() {
  /*setup serial communication*/
  Serial.begin(9600);

  /*initiallize nokia lcd library*/
  display.begin();
  display.setContrast(60);
  display.clearDisplay();

  /*initiallize compass library*/
  Wire.begin();
  compass.init();
  compass.enableDefault();

  pinMode(LED_PIN,OUTPUT);
  analogWrite(LED_PIN,190);//light on

  last_on = millis();

  drawSplash();
}

void loop() {

 calculateCompass();  //calculate compass heading reading
 
 drawCompass(); //draw this reading
 delay(100);  //delay for flickering
}

void calculateCompass(){
  compass.read(); //read compass values

//  angleActual = (atan2(compass.m.y, compass.m.z) * 180.0) / 3.14;
//  if (angleActual < 0)
//  {
//    angleActual = 360 + angleActual;
//  }
//    angleActual = 360 - angleActual; //north and south are reversed

  angleActual = compass.heading();  //calculate heading value (with -y axis in our case)

  if(last_on - millis() > FLICKER_INTERVAL && fabs(QIBLAH-angleActual) <= 10){
    /* if we are close to qiblah make some flashes*/
    last_on = millis();
    digitalWrite(8,!digitalRead(8));
  }else if (fabs(QIBLAH-angleActual) > 10){
    analogWrite(8,190);
  }

}

void drawCompass(){

   display.clearDisplay();
  /*draw the angle*/
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println((int)angleActual);
  
  drawDirections();

  /*print angle indicator inside of compass circle*/
  display.drawLine(x0,y0,x0+r*sin(angleActual/180.0*PI),y0-r*cos(angleActual/180.0*PI),BLACK);
  display.display();
}

void drawDirections(){
  /*draw the compass circle*/
  display.drawCircle(x0, y0, r, BLACK);

  /*prepare settings for drawing*/
  display.setTextSize(1);
  display.setTextColor(BLACK);

  /*draw directions*/
  display.setCursor(x0-CHAR_W/2,0);
  display.println("N");

  display.setCursor((x0+r)-CHAR_W,y0-CHAR_H/2);
  display.println("E");

  display.setCursor(x0-CHAR_W/2,y0*2-CHAR_H);
  display.println("S");

  display.setCursor((x0-r),y0-CHAR_H/2);
  display.println("W");

  /*draw qiblah indicator*/
  display.setCursor(x0+r*sin(QIBLAH/180.0*PI)-CHAR_W/2,y0-r*cos(QIBLAH/180.0*PI)-CHAR_H/2);
  display.println("Q");
}

void drawSplash(){
  /*splash screen drawer*/
    for(int i = 0 ; i < 3 ;i++){
      display.drawBitmap(0, 0,  logoArray,48,48,1); //from splash.h
      display.setTextSize(2);
      display.setCursor(46,0);
      display.println("BOS");
      display.setCursor(46,18);
      display.println("LAA");
      
      display.display();
      delay(2000);
      display.clearDisplay();
      display.display();
      delay(500);
    }  
}

