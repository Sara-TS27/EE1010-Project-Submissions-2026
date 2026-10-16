// PT100 Thermometer - Arduino Uno + 16x2 LCD
// A0 = PT100 divider junction (4.65k top), A1 = reference divider junction (4.6k top, 238||237 ohm)
// Uses the 1.1 V internal reference; the A1 arm measures the supply so neither 5 V nor 1.1 V needs to be exact.

#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);   // RS, E, D4, D5, D6, D7

// ---- Measured resistor values (from multimeter) ----
const float R_TOP1 = 4650.0;   // 4.7k on PT100 side (A0)
const float R_TOP2 = 4600.0;   // 4.7k on reference side (A1)
const float R_REF  = 118.75;   // 238 || 237 ohm

// ---- PT100 constants (IEC 60751 / Callendar-Van Dusen, T >= 0 C) ----
const float R0    = 100.0;
const float CVD_A = 3.9083e-3;
const float CVD_B = -5.775e-7;

// ---- Least-squares calibration ----
// Fitted from three points (Arduino reading -> actual temperature):
//   cold water 14.48 -> 17.45 C, room 29.34 -> 32.45 C, hot water 46.53 -> 49.20 C
// actual = CAL_SLOPE * reading + CAL_OFFSET
const float CAL_SLOPE  = 0.9902;
const float CAL_OFFSET = 3.2113;   // degrees C

const int SAMPLES = 200;       // readings averaged per update

float readAverage(uint8_t pin) {
  analogRead(pin);             // throw away first reading after switching pins
  delayMicroseconds(200);
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(pin);
  }
  return sum / (float)SAMPLES;
}

float applyCalibration(float measuredC) {
  return CAL_SLOPE * measuredC + CAL_OFFSET;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("PT100 Thermo");

  analogReference(INTERNAL);   // 1.1 V reference
  for (int i = 0; i < 20; i++) {   // let the reference settle
    analogRead(A0);
    analogRead(A1);
    delay(5);
  }
  delay(500);
  lcd.clear();
}

void loop() {
  float c0 = readAverage(A0);  // PT100 node, in ADC counts
  float c1 = readAverage(A1);  // reference node, in ADC counts

  // Supply voltage expressed in the same ADC counts, from the reference arm
  float vcc = c1 * (R_TOP2 + R_REF) / R_REF;

  lcd.setCursor(0, 0);
  if (c1 < 5 || c0 >= vcc || c0 < 1) {
    lcd.print("Check wiring    ");
    lcd.setCursor(0, 1);
    lcd.print("A0:");
    lcd.print(c0, 0);
    lcd.print(" A1:");
    lcd.print(c1, 0);
    lcd.print("    ");
    delay(1000);
    return;
  }

  // PT100 resistance
  float rpt = R_TOP1 * c0 / (vcc - c0);

  // Callendar-Van Dusen solved for T (uncalibrated reading)
  float tRaw = (-CVD_A + sqrt(CVD_A * CVD_A - 4.0 * CVD_B * (1.0 - rpt / R0))) / (2.0 * CVD_B);

  // Least-squares calibration
  float t = applyCalibration(tRaw);

  // LCD
  lcd.print("Temp: ");
  lcd.print(t, 2);
  lcd.print((char)223);        // degree symbol
  lcd.print("C    ");

  lcd.setCursor(0, 1);
  lcd.print("R: ");
  lcd.print(rpt, 2);
  lcd.print(" ohm    ");

  // Serial Monitor for debugging (9600 baud)
  Serial.print("A0 counts: ");  Serial.print(c0, 2);
  Serial.print("  A1 counts: "); Serial.print(c1, 2);
  Serial.print("  R: ");        Serial.print(rpt, 2);
  Serial.print("  Raw T: ");    Serial.print(tRaw, 2);
  Serial.print("  T: ");        Serial.println(t, 2);

  delay(500);
}
