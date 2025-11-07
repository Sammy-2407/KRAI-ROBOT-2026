#include <Arduino.h>
#include <Ps3Controller.h>

#define EN_PIN 4

// Konstanta
#define MAX_SPEED       250
#define BASE_SPEED      100
#define SPEED_INCREMENT 7
#define ACCEL_COEFF     0.3f

// Variabel global
int realSpeed = BASE_SPEED;
int lX = 0, lY = 0, rX = 0, rY = 0;

int targetM[4] = {0, 0, 0, 0};
float currentM[4] = {0.0f, 0.0f, 0.0f, 0.0f};

//Slave addresses - Motor
#define MOTOR_RF 'A'
#define MOTOR_LF 'B'
#define MOTOR_RB 'C'
#define MOTOR_LB 'D'


// === FUNCTION DEFINITIONS ===
inline void readInputs();
inline void updateMotorSpeeds();

inline void sendToSlave(char addr, int data);
inline void sendToSlave(char addr, int data1, int data2);
inline void sendToSlave(char addr, int data1, int data2, int data3);
inline void sendToSlave(char addr, int data1, int data2, int data3, int data4);

//koneksi ps 3
void onConnect() {
  Serial.println("Connected.");
}
void onDisconnect() {
  Serial.println("Disconnected.");
}

void setup() {
  Serial.begin(115200);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); // Aktifkan komunikasi (misal master mode)

  Ps3.attachOnConnect(onConnect);
  Ps3.attachOnDisconnect(onDisconnect);
   Ps3.begin("e8:6b:ea:c3:e4:3a");
  // Ps3.begin("e4:65:b8:12:2c:46");
}

void loop() {
  readInputs();
  updateMotorSpeeds();
  updateActuators();
  
  delay(1);
}

//input stick ps
inline void readInputs() {
  lX = map(Ps3.data.analog.stick.lx, -128, 127, -realSpeed, realSpeed);
  lY = map(Ps3.data.analog.stick.ly, -128, 127, realSpeed, -realSpeed);
  rX = map(Ps3.data.analog.stick.rx, -128, 127, -realSpeed, realSpeed);
  rY = map(Ps3.data.analog.stick.ry, -128, 127, realSpeed, -realSpeed);

  //Update kecepatan
  if (Ps3.data.button.r1) {
    realSpeed = min(realSpeed + SPEED_INCREMENT, MAX_SPEED);
  } else if (Ps3.data.button.r2) {
    realSpeed = max(realSpeed - 60, BASE_SPEED);
  } 
}

//Update kecepatan motor
inline void updateMotorSpeeds() { 
  float vel = min(sqrt((float)(lX * lX + lY * lY)), (float)realSpeed);
  float arah = atan2(lY, lX);
  float putar = rX * 0.4f;

  targetM[0] = vel * sin(arah - 0.785f) + putar;
  targetM[1] = vel * sin(arah + 0.785f) - putar;
  targetM[2] = -vel * sin(arah - 2.356f) + putar;
  targetM[3] = -vel * sin(arah + 2.356f) - putar;

  for (int i = 0; i < 4; i++) {
    currentM[i] += (targetM[i] - currentM[i]) * ACCEL_COEFF;
    currentM[i] = constrain(currentM[i], -realSpeed, realSpeed);
  }

  sendToSlave(MOTOR_RF, (int)currentM[0]);
  sendToSlave(MOTOR_LF, (int)currentM[1]);
  sendToSlave(MOTOR_RB, (int)currentM[2]);
  sendToSlave(MOTOR_LB, (int)currentM[3]);
}


//serial comunication
inline void sendToSlave(char addr, int data) {
  Serial.println(addr);
  Serial.println(data);
  delay(15);
}

inline void sendToSlave(char addr, int data1, int data2) {
  Serial.println(addr);
  Serial.println(data1);
  Serial.println(data2);
  delay(15);
}

inline void sendToSlave(char addr, int data1, int data2, int data3) {
  Serial.println(addr);
  Serial.println(data1);
  Serial.println(data2);
  Serial.println(data3);
  delay(15);
}

inline void sendToSlave(char addr, int data1, int data2, int data3, int data4) {
  Serial.println(addr);
  Serial.println(data1);
  Serial.println(data2);
  Serial.println(data3);
  Serial.println(data4);
  delay(15);
}
