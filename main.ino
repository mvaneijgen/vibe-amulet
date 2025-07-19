#include "debug.h"
#include "led_effects.h"
#include "melody_player.h"
#include <FastLED.h>
//--------------------------------//
// 📍 Pin Selection
//--------------------------------//
const int buttonPin =
    33; // External button on pin 33 (open by default, closes when pressed)
const int vibrationPin =
    32; // 2N2222 transistor base pin for vibration motor control
// END Pin Selection --------------//

//--------------------------------//
// 🧑‍💻 User preferences
//--------------------------------//
const int melodyPicked = 0;
const unsigned long timer = 10000;
const unsigned long buttonHoldTime = 3000;
// END User preferences  --------------//

//--------------------------------//
// 🧮 State management
//--------------------------------//
bool isMelodyPlaying = false;
bool timerAlmostDone = false;
unsigned long timerStartTime = 0;
bool systemReset = true; // Start in reset state to prevent auto-start
bool initialBlinkingDone = false;
bool startupComplete = false;
static unsigned long lastVibrationUpdate = 0;
static bool isVibrating = false;
static unsigned long buttonPressStart = 0;
static bool buttonHeld = false;
// END State management --------------//

//--------------------------------//
// ⚙️ Setup
//--------------------------------//
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP); // Enable internal pull-up resistor
  pinMode(vibrationPin, OUTPUT);
  digitalWrite(vibrationPin, LOW);

  // Initialize FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  selectMelody("longBuzz");

  // 🚨 Blink a few times when ready
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(200);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(200);
  }

  Serial.println("Vibe Amulet Ready! Press button to start timer.");
  ledOff(); // Ensure LED is off at startup
}
// END Setup  --------------//

//--------------------------------//
// ⏱️ Start timer
//--------------------------------//
void startTimer(int buttonState) {
  // Button is open by default (HIGH), closes when pressed (LOW)
  if (buttonState == LOW && timerStartTime == 0 && !isMelodyPlaying) {
    // Clear reset state on first button press
    if (systemReset) {
      systemReset = false;
      Serial.println("System reset cleared - ready for timer");
      return; // Don't start timer on this press, just clear reset
    }

    timerStartTime = millis();
    Serial.println("Timer started");
    digitalWrite(vibrationPin, HIGH);
    delay(300);
    digitalWrite(vibrationPin, LOW);
    ledBreathing();
  }
}
// END ⏱️ Start timer --------------//

//--------------------------------//
// 🎶 Melody logic
//--------------------------------//
void handleTimerElapsed() {
  Serial.println("Timer elapsed. Playing melody...");
  isMelodyPlaying = true;
  timerStartTime = 0;
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  currentEffect = OFF;
  ledFlashing(200, -1);
}

void handleTimerAlmostDone(unsigned long remaining) {
  int pulseDuration = map(remaining, timer * 0.1, 0, 50, 500);
  int pauseDuration = map(remaining, timer * 0.1, 0, 500, 50);
  unsigned long currentMillis = millis();
  if (isVibrating && currentMillis - lastVibrationUpdate >= pulseDuration) {
    Serial.println("Timer almost done.");
    ledStep();
    digitalWrite(vibrationPin, LOW);
    isVibrating = false;
    lastVibrationUpdate = currentMillis;
  } else if (!isVibrating &&
             currentMillis - lastVibrationUpdate >= pauseDuration) {
    digitalWrite(vibrationPin, HIGH);
    isVibrating = true;
    lastVibrationUpdate = currentMillis;
  }
}

void playMelodyIfTimerDone() {
  if (timerStartTime != 0) {
    unsigned long elapsed = millis();
    unsigned long remaining = timer - (elapsed - timerStartTime);

    if (remaining <= 0) {
      handleTimerElapsed();
    } else if (remaining <= timer * 0.1) {
      handleTimerAlmostDone(remaining);
    }
  }
}
// END 🎶 Melody logic  --------------//

//--------------------------------//
// 🧹 Reset state
//--------------------------------//
void resetState(int buttonState) {
  // Button is open by default (HIGH), closes when pressed (LOW)
  if (buttonState == LOW && timerStartTime == 0 && isMelodyPlaying) {
    if (!buttonHeld) {
      buttonPressStart = millis();
      buttonHeld = true;
    } else if (millis() - buttonPressStart > buttonHoldTime) {
      Serial.println("Button held down, stopping melody");
      isMelodyPlaying = false;
      timerAlmostDone = false;
      buttonHeld = false;
      systemReset = true;
      digitalWrite(vibrationPin, LOW);
      ledOff();
    }
  } else {
    buttonHeld = false;
  }
}
// END 🧹 Reset state --------------//

//--------------------------------//
// Main loop
//--------------------------------//
void loop() {
  updateLEDEffect();
  playMelody();
  int buttonState = digitalRead(buttonPin);
  printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);
  resetState(buttonState);
  startTimer(buttonState);
  playMelodyIfTimerDone();
  delay(100);
}
// END Main loop --------------//
