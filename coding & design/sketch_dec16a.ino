// =======================================
// MODE MANUAL (RC) & MODE AUTO (SENSOR)
// BTS7960 + IR + ULTRASONIC
// =======================================

// ---------- MODE BUTTON ----------
#define MODE_MANUAL_PIN 3   // D3
#define MODE_AUTO_PIN   4   // D4

// ---------- RC RECEIVER ----------
#define CH1_PIN A3
#define CH2_PIN A4
unsigned long ch1, ch2;

// ---------- SENSOR ----------
#define IR1_PIN A0
#define IR2_PIN A1
#define IR3_PIN A2

#define TRIG_PIN A5
#define ECHO_PIN A6
long duration;

// ---------- MOTOR BTS7960 ----------
#define L_PWM 11
#define L_DIR 10
#define R_PWM 9
#define R_DIR 6

// ---------- MODE ----------
enum Mode { MANUAL, AUTO };
Mode currentMode = MANUAL;

// =======================================
void setup() {
  Serial.begin(9600);

  // Mode button
  pinMode(MODE_MANUAL_PIN, INPUT_PULLUP);
  pinMode(MODE_AUTO_PIN, INPUT_PULLUP);

  // RC
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);

  // Sensor
  pinMode(IR1_PIN, INPUT_PULLUP);
  pinMode(IR2_PIN, INPUT_PULLUP);
  pinMode(IR3_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Motor
  pinMode(L_PWM, OUTPUT);
  pinMode(L_DIR, OUTPUT);
  pinMode(R_PWM, OUTPUT);
  pinMode(R_DIR, OUTPUT);

  drv_motor(0, 0);

  Serial.println("SYSTEM READY : MANUAL & AUTO MODE");
}

// =======================================
void loop() {

  // ===== MODE SELECTION =====
  if (digitalRead(MODE_MANUAL_PIN) == LOW) {
    currentMode = MANUAL;
  }
  if (digitalRead(MODE_AUTO_PIN) == LOW) {
    currentMode = AUTO;
  }

  // ===== MODE EXECUTION =====
  if (currentMode == MANUAL) {
    modeManualRC();
  } else {
    modeAutoSensor();
  }

  delay(20);
}

// =======================================
// ========== MODE MANUAL RC ===============
// =======================================
void modeManualRC() {
  ch1 = pulseIn(CH1_PIN, HIGH, 25000);
  ch2 = pulseIn(CH2_PIN, HIGH, 25000);

  int speedL = 0;
  int speedR = 0;
  String arah = "NETRAL";

  if (ch1 >= 1400 && ch1 <= 1500 && ch2 >= 1400 && ch2 <= 1500) {
    speedL = 0; speedR = 0;
  }
  else if (ch1 >= 400 && ch1 <= 700 && ch2 >= 900 && ch2 <= 1100) {
    arah = "MAJU";
    speedL = 150; speedR = 150;
  }
  else if (ch1 >= 2000 && ch1 <= 2400 && ch2 >= 1800 && ch2 <= 1900) {
    arah = "MUNDUR";
    speedL = -150; speedR = -150;
  }
  else if (ch1 >= 2350 && ch1 <= 2450) {
    arah = "KANAN";
    speedL = 150; speedR = -150;
  }
  else if (ch1 >= 450 && ch1 <= 550) {
    arah = "KIRI";
    speedL = -150; speedR = 150;
  }
  else if (ch1 >= 1400 && ch1 <= 1600 && ch2 >= 800 && ch2 <= 900) {
    arah = "MAJU-KANAN";
    speedL = 200;
    speedR = 100;
  }
  else if (ch1 >= 200 && ch1 <= 300 && ch2 >= 1450 && ch2 <= 1550) {
    arah = "MAJU-KIRI";
    speedL = 100;
    speedR = 200;
  }
  else if (ch1 >= 2600 && ch1 <= 2750 && ch2 >= 1300 && ch2 <= 1500) {
    arah = "MUNDUR-KANAN";
    speedL = -200;
    speedR = -100;
  }
  else if (ch1 >= 1100 && ch1 <= 1450 && ch2 >= 2000 && ch2 <= 2100) {
    arah = "MUNDUR-KIRI";
    speedL = -100;
    speedR = -200;
  }
  
  drv_motor(speedL, speedR);

  Serial.print("[MANUAL] CH1:");
  Serial.print(ch1);
  Serial.print(" CH2:");
  Serial.print(ch2);
  Serial.print(" => ");
  Serial.println(arah);
}

// =======================================
// ========== MODE AUTO SENSOR ==============
// =======================================
void modeAutoSensor() {
  int ir1 = digitalRead(IR1_PIN);
  int ir2 = digitalRead(IR2_PIN);
  int ir3 = digitalRead(IR3_PIN);
  int jarak = bacaJarakCm();

  if (ir1 == LOW) {
    drv_motor(-120, 120);   // putar cepat
  }
  else if (ir2 == LOW) {
    drv_motor(150, 80);    // belok kanan
  }
  else if (ir3 == LOW) {
    drv_motor(80, 150);    // belok kiri
  }
  else if (jarak > 0 && jarak < 30) {
    drv_motor(180, 180);   // maju serang
  }
  else {
    drv_motor(100, -100);  // cari musuh
  }

  Serial.print("[AUTO] IR:");
  Serial.print(ir1); Serial.print(ir2); Serial.print(ir3);
  Serial.print(" Jarak:");
  Serial.println(jarak);
}

// =======================================
// ======== BACA ULTRASONIC ================
// =======================================
int bacaJarakCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0) return 0;

  return duration * 0.034 / 2;
}

// =======================================
// ======== DRIVER MOTOR BTS7960 ===========
// =======================================
void drv_motor(int speedl, int speedr) {

  // Motor kiri
  if (speedl >= 0) {
    analogWrite(L_PWM, speedl);
    digitalWrite(L_DIR, LOW);
  } else {
    digitalWrite(L_PWM, LOW);
    analogWrite(L_DIR, abs(speedl));
  }

  // Motor kanan
  if (speedr >= 0) {
    analogWrite(R_PWM, speedr);
    digitalWrite(R_DIR, LOW);
  } else {
    digitalWrite(R_PWM, LOW);
    analogWrite(R_DIR, abs(speedr));
  }
}
