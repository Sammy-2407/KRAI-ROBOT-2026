#define R_PWM 6
#define L_PWM 5
#define enPin 4

#define Slave_Adress 'D'

//Global variabel
  
int m = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(enPin, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  digitalWrite(enPin, LOW);
}

inline void readSerialCom() {
  if (!Serial.available()) return;
  if (Serial.read() != Slave_Adress) return;
  m = Serial.parseInt();
  Serial.print("M : ");
  Serial.println(m);
}

inline void motorMove() {
  if (m > 0) {
    analogWrite(R_PWM, m);
    analogWrite(L_PWM, 0);
  } else {
    analogWrite(R_PWM, 0);
    analogWrite(L_PWM, -m);
  }
}

void loop() {
  readSerialCom();
  motorMove();
}  
