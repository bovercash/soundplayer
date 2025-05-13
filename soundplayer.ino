/*
  SOUND PLAYER
*/

/*
  This project is part of an art instalation and is publicly available 
  to assist with anyone needing to modify or repair the sound player module
*/

/*
  This project is in three parts, a main controller board, a speaker, and a six-button module.

  The main controller board is populated by an Arduino Pro Micro, a 74HC165 Shift-In Register and
  a DFPlayer Mini MP3 player module with an SD card.

  The six-button module contains six push-buttons pulled to VCC when pressed.  Each button
  is pulled to ground with a 10K on the main controller board.

*/

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>

// Serial Connection to DFPlayer Mini
SoftwareSerial softSerial(/*rx =*/14, /*tx =*/15);

// 74HC165 Pins
uint8_t LOAD = 7;
uint8_t CLOCK_ENABLE = 4;
uint8_t DATA_IN = 5;
uint8_t CLOCK = 6;

// DFPlayer
uint8_t VOLUME = 25; // 0 to 30
DFRobotDFPlayerMini dfplayer;
uint8_t BUSY_PIN = 3; // pulled low when playing 
bool WAIT_ON_BUSY = false;

// This array correlates button presses shifted in 
// to a file on the SD card.  The file played is the index of the array + 1
// e.g. if BUTTONS[3] is matches, file 0004.mp3 is played
byte BUTTONS[] = {
  B11111011,  // 0001.mp3 A
  B11110111,  // 0002.mp3 D
  B10111111,  // 0003.mp3 E
  B11011111,  // 0004.mp3 B
  B11101111,  // 0005.mp3 C
  B01111111,  // 0006.mp3 F
  B10101111,  // 0007.mp3 C+E
  B11011011,  // 0008.mp3 A+B
  B11001111,  // 0009.mp3 B+C
  B00111111,  // 0010.mp3 E+F
  B11101011,  // 0011.mp3 A+C
  B11100111,  // 0012.mp3 C+D
  B01110111,  // 0013.mp3 D+F
  B10110111,  // 0014.mp3 D+E
  B11010111,  // 0015.mp3 B+D
  B10111011   // 0016.mp3 A+E
};

void setup() {

  // Serial for debug output
  Serial.begin(115200);

  // DFPlayer Serial Connectio @ 9600 Baud
  pinMode(2, OUTPUT);
  pinMode(3, INPUT);
  softSerial.begin(9600);

  // DFPlayer BUSY pin
  pinMode(BUSY_PIN, INPUT);

  delay(2000);  // wait for components to initialize
  if (!dfplayer.begin(softSerial, /*isACK = */ true, /*doReset = */ true)) {
    Serial.println("no SD card found, cannot start up!");
    while (true) {
      delay(100);
    }
  }

  // set the player volume
  dfplayer.volume(VOLUME);

  // Setup 74HC165 connections
  pinMode(LOAD, OUTPUT);
  pinMode(CLOCK_ENABLE, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA_IN, INPUT);
}

// local state
byte incoming = B00000000;
uint8_t playing = 0;

void loop() {

  // Write pulse to load pin
  digitalWrite(LOAD, LOW);
  delayMicroseconds(5);
  digitalWrite(LOAD, HIGH);
  delayMicroseconds(5);

  // Get data from 74HC165
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK_ENABLE, LOW);
  byte newincoming = shiftIn(DATA_IN, CLOCK, LSBFIRST);
  digitalWrite(CLOCK_ENABLE, HIGH);

  if (newincoming != incoming) {
    incoming = newincoming;
    if (incoming != B11111111) {
      Serial.println(incoming, BIN);
    }
  }

  // button logic
  uint8_t selectedSound = 0; // 
  for (uint8_t i = 0; i < sizeof(BUTTONS); i++) {
    if (incoming == BUTTONS[i]) {
      // loop over each BUTTON for a match
      selectedSound = i + 1;  // determine the sound file to play
    }
  }

  // check currently playing
  if (WAIT_ON_BUSY && digitalRead(BUSY_PIN) == LOW) {
    // we're currently playing a sound
    if (selectedSound == playing) {
      // same buttom pressed as before
      dfplayer.stop();
      selectedSound = 0;
    }
  } else {
    // assume nothing is playing
    playing = 0;
  }

  if (selectedSound > 0) {
    // play the selected sound
    playing = selectedSound;
    Serial.print("Playing ");
    Serial.println(selectedSound, DEC);
    dfplayer.play(playing);
  }

  delay(200);
}
