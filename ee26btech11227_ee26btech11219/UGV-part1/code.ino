/*
 * UGV Part 1 — PWM differential drive + Bluetooth (BLE) control via Dabble
 * ESP32 DevKit + L293D + 2 TT gear motors + caster
 * App: Dabble -> Gamepad (Digital or Joystick mode)
 */

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif

// ---------------- Pins ----------------
const int IN1 = 32, IN2 = 33, ENA = 27;   // left motor  (j9, j4, j10)
const int IN3 = 25, IN4 = 26, ENB = 13;   // right motor (a4, a9, a3)

// ---------------- PWM settings ----------------
const int PWM_FREQ = 1000;    // 1 kHz
const int PWM_RES  = 8;       // duty 0..255
const int CH_A = 0, CH_B = 1; // only used on ESP32 core 2.x

const int MIN_DUTY = 110;     // below this TT motors just hum; tune if needed
const int MAX_DUTY = 255;

// If a wheel spins the wrong way, set its flag to true
const bool INVERT_LEFT  = false;
const bool INVERT_RIGHT = false;

// ---------------- Drive tuning ----------------
const float TURN_GAIN = 0.6f;          // joystick steering strength
const float RAMP_STEP = 0.08f;         // smooth acceleration
const unsigned long TICK_MS = 20;      // 50 Hz control loop
const float GEARS[] = {0.4f, 0.7f, 1.0f};
const int   NUM_GEARS = 3;

extern bool isDeviceConnected;         // set by the Dabble library

int   gear = 1;
float curL = 0, curR = 0;
bool  prevTri = false, prevCross = false;
unsigned long lastTick = 0;

// ---------------- PWM helpers (work on ESP32 core 2.x and 3.x) ----------------
void pwmSetup(int pin, int ch) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, PWM_FREQ, PWM_RES);
  (void)ch;
#else
  ledcSetup(ch, PWM_FREQ, PWM_RES);
  ledcAttachPin(pin, ch);
#endif
}

void pwmWrite(int pin, int ch, int duty) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pin, duty);
  (void)ch;
#else
  ledcWrite(ch, duty);
  (void)pin;
#endif
}

// speed: -1.0 (full reverse) .. 0 (stop) .. +1.0 (full forward)
void setMotor(int inA, int inB, int en, int ch, float speed, bool invert) {
  if (invert) speed = -speed;
  if (fabsf(speed) < 0.02f) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
    pwmWrite(en, ch, 0);
    return;
  }
  digitalWrite(inA, speed > 0 ? HIGH : LOW);    // direction
  digitalWrite(inB, speed > 0 ? LOW : HIGH);
  int duty = MIN_DUTY + (int)(fabsf(speed) * (MAX_DUTY - MIN_DUTY));
  pwmWrite(en, ch, constrain(duty, 0, MAX_DUTY)); // speed
}

void drive(float left, float right) {
  setMotor(IN1, IN2, ENA, CH_A, left,  INVERT_LEFT);
  setMotor(IN3, IN4, ENB, CH_B, right, INVERT_RIGHT);
}

float ramp(float cur, float target) {
  if (target > cur + RAMP_STEP) return cur + RAMP_STEP;
  if (target < cur - RAMP_STEP) return cur - RAMP_STEP;
  return target;
}

// ---------------- Drive system ----------------
void readGamepad(float &tL, float &tR) {
  tL = tR = 0;

  // Triangle = faster gear, Cross = slower gear
  bool tri = GamePad.isTrianglePressed(), cross = GamePad.isCrossPressed();
  if (tri && !prevTri && gear < NUM_GEARS - 1) gear++;
  if (cross && !prevCross && gear > 0) gear--;
  prevTri = tri; prevCross = cross;

  // Joystick mode
  if (GamePad.getRadius() > 0) {
    float y = GamePad.getYaxisData() / 7.0f;
    float x = GamePad.getXaxisData() / 7.0f * TURN_GAIN;
    tL = y + x;
    tR = y - x;
    float m = max(fabsf(tL), fabsf(tR));
    if (m > 1.0f) { tL /= m; tR /= m; }
    return;
  }

  // Digital (D-pad) mode
  float g = GEARS[gear];
  bool up = GamePad.isUpPressed(), down = GamePad.isDownPressed();
  bool left = GamePad.isLeftPressed(), right = GamePad.isRightPressed();

  if (up)        { tL = g;  tR = g;  }
  else if (down) { tL = -g; tR = -g; }

  if (left) {
    if (up || down) tL *= 0.3f;
    else { tL = -0.6f * g; tR = 0.6f * g; }
  }
  if (right) {
    if (up || down) tR *= 0.3f;
    else { tL = 0.6f * g; tR = -0.6f * g; }
  }

  if (GamePad.isSquarePressed()) { tL = -g; tR = g; }  // spin left
  if (GamePad.isCirclePressed()) { tL = g;  tR = -g; } // spin right
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pwmSetup(ENA, CH_A);
  pwmSetup(ENB, CH_B);
  drive(0, 0);

  Dabble.begin("UGV_ESP32");
  Serial.println("UGV ready - connect from the Dabble app");
}

void loop() {
  Dabble.processInput();

  if (millis() - lastTick < TICK_MS) return;
  lastTick = millis();

  if (!isDeviceConnected) {      // failsafe: phone disconnected -> stop
    curL = curR = 0;
    drive(0, 0);
    return;
  }

  float tL, tR;
  readGamepad(tL, tR);
  curL = ramp(curL, tL);
  curR = ramp(curR, tR);
  drive(curL, curR);

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 300) {
    lastPrint = millis();
    Serial.printf("gear=%d  L=%+.2f  R=%+.2f\n", gear + 1, curL, curR);
  }
}
