


#include "Arduino.h"


#include "vc.h"

#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

#define LEDPIN 13  // LEDPIN is a constant








void setup() {
	// start serial port at 9600 bps:
  Serial.begin(115200);



  pinMode(LEDPIN, OUTPUT); // LED init
  digitalWrite(LEDPIN, 1); // write inversed state back
}

void toggleLED() {
  boolean ledstate = digitalRead(LEDPIN); // get LED state
  ledstate ^= 1;   // toggle LED state using xor
  digitalWrite(LEDPIN, ledstate); // write inversed state back
}

void loop() {



}
