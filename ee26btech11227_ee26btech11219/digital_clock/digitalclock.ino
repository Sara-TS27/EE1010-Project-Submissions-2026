#include <Arduino.h>

// =====================================================
// 6 DIGIT DIGITAL CLOCK + STOPWATCH
// Arduino UNO + 74LS47
// =====================================================

// ---------------- DISPLAY CONNECTIONS -----------------

// 7447 BCD input pins
const int bcdPins[4] = {0, 1, 2, 3};

// Six display enable pins
const int digitPins[6] = {4, 5, 6, 7, 8, 9};

// ---------------- BUTTON CONNECTIONS ------------------

// Button 1: Hour in clock / Start-Stop in stopwatch
const int hourButton = 10;

// Button 2: Minute in clock / Reset in stopwatch
const int minuteButton = 11;

// Button 3: Clock <-> Stopwatch
const int modeButton = 12;


// ---------------- CLOCK VARIABLES ---------------------

int hours = 12;
int minutes = 0;
int seconds = 0;

unsigned long previousClockMillis = 0;


// ---------------- STOPWATCH VARIABLES -----------------

bool stopwatchMode = false;
bool stopwatchRunning = false;

unsigned long stopwatchStartMillis = 0;
unsigned long stopwatchElapsed = 0;


// ---------------- BUTTON VARIABLES --------------------

bool lastHourButtonState = LOW;
bool lastMinuteButtonState = LOW;
bool lastModeButtonState = LOW;

unsigned long lastHourPress = 0;
unsigned long lastMinutePress = 0;
unsigned long lastModePress = 0;

const unsigned long debounceTime = 200;


// =====================================================
// SETUP
// =====================================================

void setup() {

  // 7447 BCD pins
  for (int i = 0; i < 4; i++) {
    pinMode(bcdPins[i], OUTPUT);
  }

  // Display enable pins
  for (int i = 0; i < 6; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW);
  }

  // Buttons
  // Your wiring uses external 10k pull-down resistors.
  pinMode(hourButton, INPUT);
  pinMode(minuteButton, INPUT);
  pinMode(modeButton, INPUT);
}


// =====================================================
// WRITE BCD TO 7447
// =====================================================

void writeBCD(int val) {

  for (int i = 0; i < 4; i++) {
    digitalWrite(bcdPins[i], (val >> i) & 0x01);
  }
}


// =====================================================
// DISPLAY SIX DIGITS
// =====================================================

void displayDigits(int digits[6]) {

  for (int i = 0; i < 6; i++) {

    // Send BCD value
    writeBCD(digits[i]);

    // Turn display ON
    digitalWrite(digitPins[i], HIGH);

    // Short multiplexing delay
    delay(1);

    // Turn display OFF
    digitalWrite(digitPins[i], LOW);
  }
}


// =====================================================
// DISPLAY CLOCK
// =====================================================

void displayClock() {

  int digits[6];

  digits[0] = hours / 10;
  digits[1] = hours % 10;

  digits[2] = minutes / 10;
  digits[3] = minutes % 10;

  digits[4] = seconds / 10;
  digits[5] = seconds % 10;

  displayDigits(digits);
}


// =====================================================
// DISPLAY STOPWATCH
// Format: MM:SS:CC
// CC = centiseconds (1/100 second)
// =====================================================

void displayStopwatch() {

  unsigned long elapsed = stopwatchElapsed;

  // If stopwatch is running, calculate current elapsed time
  if (stopwatchRunning) {
    elapsed = millis() - stopwatchStartMillis;
  }

  unsigned long totalCentiseconds = elapsed / 10;

  int stopwatchMinutes =
      (totalCentiseconds / 6000) % 100;

  int stopwatchSeconds =
      (totalCentiseconds / 100) % 60;

  int centiseconds =
      totalCentiseconds % 100;

  int digits[6];

  digits[0] = stopwatchMinutes / 10;
  digits[1] = stopwatchMinutes % 10;

  digits[2] = stopwatchSeconds / 10;
  digits[3] = stopwatchSeconds % 10;

  digits[4] = centiseconds / 10;
  digits[5] = centiseconds % 10;

  displayDigits(digits);
}


// =====================================================
// UPDATE NORMAL CLOCK
// =====================================================

void updateClock() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousClockMillis >= 1000) {

    previousClockMillis += 1000;

    seconds++;

    if (seconds >= 60) {
      seconds = 0;
      minutes++;

      if (minutes >= 60) {
        minutes = 0;
        hours++;

        if (hours >= 24) {
          hours = 0;
        }
      }
    }
  }
}


// =====================================================
// HANDLE HOUR BUTTON
// =====================================================

void handleHourButton() {

  bool currentState = digitalRead(hourButton);

  // Detect LOW -> HIGH transition
  if (currentState == HIGH &&
      lastHourButtonState == LOW &&
      millis() - lastHourPress > debounceTime) {

    lastHourPress = millis();

    if (!stopwatchMode) {

      // CLOCK MODE:
      // Increase hour
      hours++;

      if (hours >= 24) {
        hours = 0;
      }

    } else {

      // STOPWATCH MODE:
      // Start / Stop

      if (!stopwatchRunning) {

        // Start/resume stopwatch
        stopwatchStartMillis =
            millis() - stopwatchElapsed;

        stopwatchRunning = true;

      } else {

        // Stop stopwatch
        stopwatchElapsed =
            millis() - stopwatchStartMillis;

        stopwatchRunning = false;
      }
    }
  }

  lastHourButtonState = currentState;
}


// =====================================================
// HANDLE MINUTE BUTTON
// =====================================================

void handleMinuteButton() {

  bool currentState = digitalRead(minuteButton);

  // Detect LOW -> HIGH transition
  if (currentState == HIGH &&
      lastMinuteButtonState == LOW &&
      millis() - lastMinutePress > debounceTime) {

    lastMinutePress = millis();

    if (!stopwatchMode) {

      // CLOCK MODE:
      // Increase minute
      minutes++;

      if (minutes >= 60) {
        minutes = 0;
      }

    } else {

      // STOPWATCH MODE:
      // Reset stopwatch

      stopwatchRunning = false;
      stopwatchElapsed = 0;
      stopwatchStartMillis = millis();
    }
  }

  lastMinuteButtonState = currentState;
}


// =====================================================
// HANDLE MODE BUTTON
// =====================================================

void handleModeButton() {

  bool currentState = digitalRead(modeButton);

  // Detect LOW -> HIGH transition
  if (currentState == HIGH &&
      lastModeButtonState == LOW &&
      millis() - lastModePress > debounceTime) {

    lastModePress = millis();

    // Toggle mode
    stopwatchMode = !stopwatchMode;

    // If returning to clock mode,
    // stop stopwatch but keep its value.
    if (!stopwatchMode) {

      if (stopwatchRunning) {
        stopwatchElapsed =
            millis() - stopwatchStartMillis;

        stopwatchRunning = false;
      }
    }
  }

  lastModeButtonState = currentState;
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Keep normal clock running in the background
  updateClock();

  // Check all buttons
  handleHourButton();
  handleMinuteButton();
  handleModeButton();

  // Display whichever mode we're currently in
  if (stopwatchMode) {
    displayStopwatch();
  } else {
    displayClock();
  }
}
