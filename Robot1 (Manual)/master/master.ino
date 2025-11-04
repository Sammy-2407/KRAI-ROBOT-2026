#include <Ps3Controller.h>
#include "Gerak_Roda.h"
#include "Tombol.h"
#define enPin 4

void onConnect() {
  Serial.println("Connected.");
}
void onDisconnect() {
  Serial.println("Disconnected.");
}

// Analog
int baseSpeed = 100;
int realSpeed;
int spdShooter = 230;
int spdLift = 220;  // update speed lift pre visitasi
// int m1, m2, m3, m4;

void setup() {
  Serial.begin(115200);

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, HIGH); // Master mode

  Ps3.attachOnConnect(onConnect);
  Ps3.attachOnDisconnect(onDisconnect);
  Ps3.begin("e4:65:b8:12:2c:46");
  // Ps3.begin("e8:6b:ea:c3:e4:3a");
}

void loop() {
  tombol(spdLift, spdShooter);
  speed(baseSpeed);
  gerak(realSpeed);
  // autorun1();
 delay(1);
}

void speed(int baseSpeed) {
  if (Ps3.data.button.r1) {
    if(realSpeed > 250){
      realSpeed = 250;
    }else{
      realSpeed += 7;
    }
  } 
  else if (Ps3.data.button.r2) {
    realSpeed = baseSpeed-60;
  }
  else{
    realSpeed = baseSpeed;
  }
}
