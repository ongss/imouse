 // USE FOR WIFI
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
ESP8266WiFiMulti WiFiMulti;

const String ip = "172.20.10.9";

// USE FOR GY-521
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

MPU6050 accelgyro;

int16_t ax, ay, az, gx, gy, gz;

double timeStep, timeCur, timePrev;
double arx, ary, arz, grx, gry, grz, gsx, gsy, gsz, rx, ry, rz;
int X,Y,Z;
int newX,newY,newZ;


//int i;
double gyroScale = 131;

// USE FOR LEFT AND RIGHT CLICK

int leftClick = 14; // D5
int rightClick = 12; // D6
int calibrate = 13; // D7

bool leftCur = false;
bool leftPvr = false;

bool rightCur = false;
bool rightPvr = false;

bool calibrateCur = false; 
bool calibratePvr = false;

int caliX = 0;
int caliY = 0;
int caliZ = 0;


void setup() {
  
  // SET UP GY-521
  Wire.begin();
  Serial.begin(9600);
  accelgyro.initialize();

  // SET UP WIFI
  for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
  }

   //WiFiMulti.addAP("Asus","qwerty123");
   //WiFiMulti.addAP("AndroidAP","nqxq0952");
   //WiFiMulti.addAP("siamgamss", "culaw271");
   WiFiMulti.addAP("JTiphone","12345678");
   //WiFiMulti.addAP("Punsa<3","ktpunisa");
   //WiFiMulti.addAP(wifiName,wifiPwd);
    //WiFiMulti.addAP("OakChawit", "ZymphonY");
   
   Serial.println("Wifi Connecting");
   while(WiFiMulti.run() != WL_CONNECTED){
      Serial.print(" .");
      delay(1000);
   }
   Serial.println("\nWifi Connected");
    

  //SET UP LEFT AND RIGHT CLICK
  pinMode(leftClick,INPUT);
  pinMode(rightClick,INPUT);
  
}

void loop() {
  WiFiMulti.run();
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // calculate accelerometer angles
  arx = (180/3.141592) * atan(ax / sqrt(pow(ay, 2) + pow(az, 2))); 
  ary = (180/3.141592) * atan(ay / sqrt(pow(ax, 2) + pow(az, 2)));
  arz = (180/3.141592) * atan(sqrt(pow(ay,2) + pow(ax,2)) / az);

  X = (int) (arx+3);
  Y = (int) (ary+1.8);
  Z = (int) (arz+4);

//  Serial.print(X);   Serial.print("\t");
//  Serial.print(Y);   Serial.print("\t");
//  Serial.print(Z);   Serial.print("\t\t");
//  Serial.println();

//    Serial.print(caliX); Serial.print("\t");
//    Serial.print(caliY); Serial.print("\t");
//    Serial.println(caliZ); Serial.print("\t");
  
  if(abs(X-caliX) <= 4) newX = 0;
  else if(X-caliX >= 0) newX = X-caliX-4;
  else if(X-caliX < 0 ) newX = X-caliX+4;
  if(abs(Y-caliY) <= 4) newY = 0;
  else if(Y-caliY >= 0) newY = Y-caliY-4;
  else if(Y-caliY < 0) newY = Y-caliY+4;
  if(abs(Z-caliZ) <= 4) newZ = 0;
  else if(Z-caliZ >= 0) newZ = Z-caliZ-5;
  else if(Z-caliZ < 0) newZ = Z-caliZ+4;
  Serial.print(newX);   Serial.print("\t");
  Serial.print(newY);   Serial.print("\t");
  Serial.print(newZ);   Serial.print("\t\t");
  Serial.println();

  HTTPClient http;    //Declare object of class HTTPClient

  // MOUSE CLICK CONTROLLER
  leftPvr = leftCur;
  leftCur = (digitalRead(leftClick) == HIGH);
  
  if(leftPvr == false && leftCur == true){
    Serial.println("left pushed");
    http.begin("http://"+ip+":3000/left-click");      //Specify request destination
    int httpCode = http.POST("hello");   //Send the request
  }

  rightPvr = rightCur;
  rightCur = (digitalRead(rightClick) == HIGH);

  if(rightPvr == false && rightCur == true){
    Serial.println("right pushed");
    http.begin("http://"+ip+":3000/right-click");      //Specify request destination
    int httpCode = http.POST("hello");   //Send the request
   }

  
   //MOUSE POS CONTROLLER
  String strx,stry,str;
  
  http.begin("http://"+ip+":3000/move-by");      //Specify request destination
  http.addHeader("Content-Type","application/x-www-form-urlencoded");  //Specify content-type header
  strx = String(-(newX)*2.5);
  stry = String((newY)*1.9);
  str = String();
  str = "x=" + strx +"&y=" + stry;
  int httpCode = http.POST(str);   //Send the request


  

  calibratePvr = calibrateCur;
  calibrateCur = (digitalRead(calibrate) == HIGH);

  if(calibratePvr == false && calibrateCur == true){
    Serial.println("calibrate pushed");
    if((X < 45 && X > -45) && (Y < 45 && Y > -45)){
        caliX = X;
        caliY = Y;
        caliZ = Z;  
    }
    else Serial.println("calibate isn't avalible");
  }

  delay(10);
  
}

