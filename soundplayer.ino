#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/14, /*tx =*/15);

uint8_t load = 7;
uint8_t clockEnablePin = 4;
uint8_t dataIn = 5;
uint8_t clockIn = 6;

DFRobotDFPlayerMini myDFPlayer;
uint8_t busyPin = 3;
bool waitBusy = false;

void setup(){
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  softSerial.begin(9600); // DFPlayer

  pinMode(busyPin, INPUT);
  
  delay(2000);
  if (!myDFPlayer.begin(softSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    while(true){
      delay(100);
    }
    
  }
  Serial.println("DFPlayer Connected!");
  myDFPlayer.volume(25);  //Set volume value. From 0 to 30

  // 74HC165 connections
  pinMode(load, OUTPUT);
  pinMode(clockEnablePin, OUTPUT);
  pinMode(clockIn, OUTPUT);
  pinMode(dataIn, INPUT);

  Serial.println("Starting!");
}

byte incoming = B00000000;

void loop(){

  // Write pulse to load pin
  digitalWrite(load, LOW);
  delayMicroseconds(5);
  digitalWrite(load, HIGH);
  delayMicroseconds(5);

  // Get data from 74HC165
  digitalWrite(clockIn, HIGH);
  digitalWrite(clockEnablePin, LOW);
  byte newincoming = shiftIn(dataIn, clockIn, LSBFIRST);
  digitalWrite(clockEnablePin, HIGH);

  if(newincoming != incoming) {
    incoming = newincoming;
    Serial.print("Pin States: ");
    Serial.println(incoming, BIN);
  }



  if (!waitBusy || digitalRead(busyPin) == HIGH) {
    switch (incoming) {
      case B11111111:
        // default state
        break;

      case B11110111:
        Serial.println("Playing 2");
        myDFPlayer.play(1);
        break;

      case B11111011:
        Serial.println("Playing 2");
        myDFPlayer.play(2);
        break;

      default:
        break;
    }
  }

  delay(200);
}

