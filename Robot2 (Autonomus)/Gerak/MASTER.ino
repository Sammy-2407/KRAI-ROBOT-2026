#include <ModbusRtu.h>

#define EN_PIN     4
#define MASTER_ID  0

#define MIN_SPEED  20
#define MAX_SPEED  30
#define BASE_SPEED 21

#define LIMIT_SWITCH 18


#define ARUCO_CAM 19

uint16_t dataReadX[2];    // Read X encoder
uint16_t dataReadY[2];    // Read Y encoder

uint16_t frontMotorReg[2];
uint16_t rearMotorReg[2];

uint16_t putarReg[1];
uint16_t gripReg[1];

uint16_t jarakReg[1];

uint16_t arucoReg[1];

//uint16_t servoReg[2] = {0,0};

HardwareSerial Raspi(2);

bool markerDetected = false;




// Modbus master
Modbus    master(MASTER_ID, Serial, EN_PIN);
modbus_t  telegram[12]; //2

float posX = 0;
float posY = 0;
float yaw = 0;

// Target Variable
float targetXMaju = -200.0f;

float targetXMundur = -50.0f;


float targetY = -30.0f;   

// PIDX
float KpX = 0.6;
float KiX = 0.0;
float KdX = 0.4;
float integralX = 0;
float prevErrorX = 0;

// PIDY
float KpY = 0.6;
float KiY = 0.0;
float KdY = 0.4;
float integralY = 0;
float prevErrorY = 0;

float scale = 1.0; //scalling

// Angular 
float KpAngular = 0.65;
float KiAngular = 0;
float KdAngular = 0.1;
float integralAngular = 0;
float prevErrorAngular = 0;

// Motor Speed Variable
float motorSpeed[4] = {0.0f, 0.0f, 0.0f, 0.0f};

unsigned long timer = 0;

enum RobotState
{
  STATE_INIT,
  STATE_GERAK_MAJU,
  STATE_GERAK_MUNDUR,
  STATE_GERAK_Y,
  STATE_ROTASI_CW,
  STATE_ROTASI_CCW,
  PUTAR_LENGAN,
  AMBIL,
  CARI_TOMBAK,
  BUKA_GRIP,
  STATE_FINISH
};

RobotState state = STATE_INIT;


void setup()
{
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  pinMode(ARUCO_CAM, INPUT);     //mauskan raspio
  pinMode(LIMIT_SWITCH, INPUT_PULLUP); // limit switch

  Serial.begin(115200);

  master.start();
  master.setTimeOut(200);
}


void loop()
{
  // Read slave X
  readSlaveX();
  posX = convertToFloatX();

  // Read slave Y
  readSlaveY();
  posY = convertToFloatY();



  bacaJarak();
  aruco();

  if(digitalRead(ARUCO_CAM) == HIGH)
    {
        sendServoCommand(2);
    }

  uint16_t jarakRak = jarakReg[0];

  switch(state)
  {
    case STATE_INIT:
    {
      stopAllMotor();
      delay(500);

                                                                                                    state = STATE_GERAK_MAJU;
      break;
    }
   
    case STATE_GERAK_MAJU:
    {
      
      float errorX = targetXMaju - posX;
      // float rotationY = posY;

      // PID Encoder
      float vx = PID_X(errorX);

      if (vx > 0) vx += BASE_SPEED;
      else vx -= BASE_SPEED;

      if(limitTriggered()) // Treshold
      {
        stopAllMotor();
        odometriBiasa(0, 0, 0, 0);
        delay(300);

        resetVariable();
                                                                                                                        state = CARI_TOMBAK; 
      } else {
        odometriBiasa(vx, 0, 0, 0);
      }
      break;
    }

    case CARI_TOMBAK:
      {
          bacaJarak();

          if(jarakReg[0] == 1)
          {
            // sendFrontMotor(0, 0);
            // sendRearMotor(0, 0);
            stopAllMotor();

            odometriBiasa(0, 0, 0, 0);

              delay(500);

                                                                                 state = AMBIL;
                                                                                
          }
          else
          {
              // Geser ke kanan
              // odometriBiasa(0,-1,0,0);
              if(!limitTriggered()) //luruskan
                {
                  odometriBiasa(-10, 0, 0, 0); 
                }else{
                  // odometriBiasa(0,-1,0,0);

                  sendFrontMotor(-50, 15);
                  sendRearMotor(-50, 15);
                }
          }

          break;
      }

     
    case BUKA_GRIP:
      {
        // if (digitalRead(ARUCO_CAM) == HIGH){
        //   sendServoCommand(1);

        //   state = STATE_FINISH;
        // }

        static bool sent = false;
        static unsigned long timerBuka;

        if (aruco()){
          // sendServoCommand(2);
          // delay(500);

          // state = STATE_FINISH;

          if(!sent)
            {
              sendServoCommand(2);

              timerBuka = millis();
              sent = true;
            }

            if(millis() - timerBuka > 1000)
            {
              sent = false;

                                                                                                        state = STATE_FINISH;
            }
            }



        break;
      }

    
    // case STATE_ROTASI_CCW:
    // {
    //   static bool triggerTimer = false;

    //   if(!triggerTimer)
    //   {
    //     timer = millis();
    //     triggerTimer = true;
    //   }
    //   odometriBiasa(0, 0, 0, -30); ///atur +- ny

    //   if (millis() - timer > 3000) {
    //     triggerTimer = false;
    //     stopAllMotor();
    //     state = STATE_SERVO_GRIP;
    //   }
    //   break;
    // }

    case AMBIL:
      {
        static bool sent = false;
        static unsigned long timerGrip;

        if(!sent)
        {
          sendServoCommand(1);

          timerGrip = millis();
          sent = true;
        }

        if(millis() - timerGrip > 2500)
        {
          sent = false;

                                                                                                    state = PUTAR_LENGAN;
        }

        break;
      }

    case PUTAR_LENGAN:
      {
        static bool commandSent = false;
        static unsigned long timerPutar = 0;

        if(!commandSent)
        {
          sendLifterCommand(1); // naik

          timerPutar = millis();
          commandSent = true;
        }

        if(millis() - timerPutar > 5000)
        {
          commandSent = false;

                                                                                state = BUKA_GRIP;//STATE_FINISH;//STATE_GERAK_MUNDUR;
        }

        break;
      }

    case STATE_FINISH:
    {
      stopAllMotor();  
      break;
    }
  }
  delay(20);
}

 
void resetVariable() {
  // Reset Integral
  integralX = 0;
  integralY = 0;
  integralAngular = 0;

  // Reset Previous Error
  prevErrorX = 0;
  prevErrorY = 0;
  prevErrorAngular = 0;
}

 
void odometriBiasa(float vx, float vy, float arah, float putar) {
  int16_t LF = vx + vy + putar;
  int16_t RF = vx - vy - putar;
  int16_t LB = vx + vy + putar;
  int16_t RB = vx - vy - putar;

  // Set Limit Max Speed
  LF = constrain(LF, -MAX_SPEED, MAX_SPEED);
  RF = constrain(RF, -MAX_SPEED, MAX_SPEED);
  LB = constrain(LB, -MAX_SPEED, MAX_SPEED);
  RB = constrain(RB, -MAX_SPEED, MAX_SPEED);

  // Set Minimal Limit Speed
  if (abs(LF) < MIN_SPEED && LF != 0) LF = (LF > 0) ? MIN_SPEED : -MIN_SPEED;
  if (abs(RF) < MIN_SPEED && RF != 0) RF = (RF > 0) ? MIN_SPEED : -MIN_SPEED;
  if (abs(LB) < MIN_SPEED && LB != 0) LB = (LB > 0) ? MIN_SPEED : -MIN_SPEED;
  if (abs(RB) < MIN_SPEED && RB != 0) RB = (RB > 0) ? MIN_SPEED : -MIN_SPEED;

  sendFrontMotor(LF, RF);
  sendRearMotor(LB, RB);
}

void mecanumMove(float vx, float vy)
{
  int16_t LF = vx - vy;
  int16_t RF = vx + vy;

  int16_t LB = vx + vy;
  int16_t RB = vx - vy;

  LF = constrain(LF, -120, 120);
  RF = constrain(RF, -120, 120);

  LB = constrain(LB, -120, 120);
  RB = constrain(RB, -120, 120);

  sendFrontMotor(LF, RF);
  sendRearMotor(LB, RB);
}

void stopAllMotor()
{
  sendFrontMotor(0, 0);
  sendRearMotor(0, 0);
}



//PID kontrol
float PID_X(float error)
{
  integralX += error;
  integralX = constrain(integralX, -100, 100);

  float derivative = error - prevErrorX;

  prevErrorX = error;

  float absError = fabs(error);
  scale = 1.0;

  if (absError < 90) scale = 0.2;
  if (absError < 50) scale = 0.2;
  if (absError < 20) scale = 0.25;
  if (absError < 10) scale = 0.1;

  // Hasil PID ke kecepatan
  float PID = KpX*error + KiX*integralX + KdX*derivative;
  PID = constrain(PID * scale, -MAX_SPEED, MAX_SPEED);

  return PID;
}

float PID_Y(float error)
{
  integralY += error;
  integralY = constrain(integralY, -100, 100); // sesuaikan limitnya


  float derivative = error - prevErrorY;

  prevErrorY = error;

  return
      KpY*error +
      KiY*integralY +
      KdY*derivative;
}

float PID_Angular(float error) {
  integralAngular += error;
  // integralAngular = constrain(integralAngular, -100, 100); // sesuaikan limitnya

  float derivative = error - prevErrorAngular;

  prevErrorAngular = error;

  float absError = fabs(error);
  scale = 1.0;

  // if (absError < 50) scale = 0.85;
  // if (absError < 30) scale = 0.6;
  // if (absError < 15) scale = 0.30;
  if (absError < 10) scale = 0.2;

  // Hasil PID ke kecepatan
  float PID = KpX*error + KiX*integralX + KdX*derivative;
  PID = constrain(PID * scale, -MAX_SPEED, MAX_SPEED);

  return PID;
}


//covert ke float 
float convertToFloatX()
{
  union
  {
    float f;
    uint16_t reg[2];
  } data;

  data.reg[0] = dataReadX[0];
  data.reg[1] = dataReadX[1];

  return data.f;
}

float convertToFloatY()
{
  union
  {
    float f;
    uint16_t reg[2];
  } data;

  data.reg[0] = dataReadY[0];
  data.reg[1] = dataReadY[1];

  return data.f;
}


// Encoder Register
bool readSlaveX()
{
  telegram[0].u8id       = 1;
  telegram[0].u8fct      = 3;      // Read Holding Register
  telegram[0].u16RegAdd  = 0;      // Mulai register 0
  telegram[0].u16CoilsNo = 2;      // Baca 2 register
  telegram[0].au16reg    = dataReadX;

  master.query(telegram[0]);

  unsigned long timeout = millis();

  while (master.getState() != COM_IDLE)
  {
    master.poll();

    if (millis() - timeout > 500)
    {
      Serial.println("Timeout komunikasi");
      return false;
    }
  }

  return true;
}

bool readSlaveY()
{
  telegram[1].u8id       = 2;
  telegram[1].u8fct      = 3;      // Read Holding Register
  telegram[1].u16RegAdd  = 0;      // Mulai register 0
  telegram[1].u16CoilsNo = 2;      // Baca 2 register
  telegram[1].au16reg    = dataReadY;

  master.query(telegram[0]);

  unsigned long timeout = millis();

  while (master.getState() != COM_IDLE)
  {
    master.poll();

    if (millis() - timeout > 500)
    {
      Serial.println("Timeout komunikasi");
      return false;
    }
  }

  return true;
}

// Drivetrain Register
void sendFrontMotor(int16_t LF, int16_t RF)
{
  frontMotorReg[0] = LF;
  frontMotorReg[1] = RF;

  telegram[3].u8id = 3;
  telegram[3].u8fct = 16;
  telegram[3].u16RegAdd = 0;
  telegram[3].u16CoilsNo = 2;
  telegram[3].au16reg = frontMotorReg;

  master.query(telegram[3]);

  while(master.getState()!=COM_IDLE)
    master.poll();
    delay(15);

    
}

void sendRearMotor(int16_t LB, int16_t RB)
{
  rearMotorReg[0] = LB;
  rearMotorReg[1] = RB;

  telegram[2].u8id = 4;
  telegram[2].u8fct = 16;
  telegram[2].u16RegAdd = 0;
  telegram[2].u16CoilsNo = 2;
  telegram[2].au16reg = rearMotorReg;

  master.query(telegram[2]);

  while(master.getState()!=COM_IDLE)
    master.poll();
    delay(15);
}

void sendLifterCommand(uint16_t cmd)
{
  putarReg[0] = cmd;

  telegram[4].u8id = 5;
  telegram[4].u8fct = 16;
  telegram[4].u16RegAdd = 0;
  telegram[4].u16CoilsNo = 1;
  telegram[4].au16reg = putarReg;

  master.query(telegram[4]);

  while(master.getState() != COM_IDLE)
    master.poll();
}

void sendServoCommand(uint16_t cmd)
{
  gripReg[0] = cmd;

  telegram[5].u8id       = 7;
  telegram[5].u8fct      = 16;
  telegram[5].u16RegAdd  = 0;
  telegram[5].u16CoilsNo = 1;
  telegram[5].au16reg    = gripReg;

  master.query(telegram[5]);

  while(master.getState() != COM_IDLE)
    master.poll();
}


bool bacaJarak()
{
  telegram[6].u8id       = 5;
  telegram[6].u8fct      = 3;
  telegram[6].u16RegAdd  = 1;
  telegram[6].u16CoilsNo = 1;
  telegram[6].au16reg    = jarakReg;

  master.query(telegram[6]);

  unsigned long timeout = millis();

  while(master.getState()!=COM_IDLE)
  {
    master.poll();

    if(millis()-timeout > 300)
      return false;
  }

  return true;
}

bool limitTriggered()
{
    if(digitalRead(LIMIT_SWITCH) == LOW)
    {
        delay(10);

        return digitalRead(LIMIT_SWITCH) == LOW;
    }

    return false;
}

bool aruco()
{
    if(digitalRead(ARUCO_CAM) == HIGH)
    {
        delay(10);

        return digitalRead(LIMIT_SWITCH) == HIGH;
    }

    return false;
}
