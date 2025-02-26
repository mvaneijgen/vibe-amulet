#include "debug.h" // Include the debug header
#include "led_effects.h"
#include "melody/melody_player.h"
//--------------------------------//
// 📍 Pin Selection
//--------------------------------//
const int buttonPin = 0;
const int vibrationPin = 13;
// END Pin Selection --------------//

//--------------------------------//
// 🧑‍💻 User prefrences
//--------------------------------//
const int melodyPicked = 0;
const unsigned long timer = 10000;
const unsigned long buttonHoldTime = 3000;
// END User prefrences  --------------//

//--------------------------------//
// 🧮 State managment
//--------------------------------//
bool isMelodyPlaying = false;
bool timerAlmostDone = false;
unsigned long timerStartTime = 0;
bool systemReset = false; // Add a flag to indicate system reset
bool initialBlinkingDone =
    false;                    // Add a flag to indicate initial blinking is done
bool startupComplete = false; // New flag to track startup sequence

static unsigned long lastVibrationUpdate = 0;
static bool isVibrating = false;
static unsigned long buttonPressStart = 0;
static bool buttonHeld = false;
// END State managment --------------//

const unsigned long breathCycle = 2000;

//--------------------------------//
// ⚙️ Setup
//--------------------------------//
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(vibrationPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(vibrationPin, LOW);
  // 🚨 Blink a few times when ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}
// END Setup  --------------//

void loop() {
  // Load external scripts
  updateLEDEffect();
  playMelody();

  int buttonState = digitalRead(buttonPin);
  printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);

  //--------------------------------//
  // 👆 Enable reset button on hold
  //--------------------------------//
  if (buttonState == LOW && timerStartTime == 0 && isMelodyPlaying) {
    if (!buttonHeld) {
      buttonPressStart = millis();
      buttonHeld = true;
    } else if (millis() - buttonPressStart > buttonHoldTime) {
      Serial.println("Button held down, stopping melody");
      // Reset state back to nomral
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
  // END 👆 Enable reset button on hold --------------//

  //--------------------------------//
  // Start ⏱️ timer
  //--------------------------------//
  if (buttonState == LOW && timerStartTime == 0 && !isMelodyPlaying &&
      !systemReset) {
    timerStartTime = millis();
    Serial.println("Timer started");
    digitalWrite(vibrationPin, HIGH);
    delay(300);
    digitalWrite(vibrationPin, LOW);
    ledBreathing();
  }
  // END Start ⏱️ timer  --------------//

  //--------------------------------//
  // 🎶 Play melody if timer is done
  //--------------------------------//
  if (timerStartTime != 0) {
    unsigned long elapsed = millis();
    unsigned long remaining = timer - (elapsed - timerStartTime);

    if (remaining <= 0) {
      Serial.println("Timer elapsed. Playing melody...");
      isMelodyPlaying = true;
      timerStartTime = 0;
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, LOW);
      currentEffect = OFF;
      ledFlashing(200, -1);
    } else if (remaining <= timer * 0.1) {

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
  }
  // END 🎶 Play melody if timer is done --------------//
  delay(100);
}
