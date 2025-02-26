#include "led_effects.h"
#include "melody/melody_player.h"
#include "debug.h"  // Include the debug header
//--------------------------------//
// Pin Selection
//--------------------------------//
const int buttonPin = 0;
const int vibrationPin = 13;
// END Pin Selection --------------//

//--------------------------------//
// User prefrences
//--------------------------------//
const int melodyPicked = 0;
const unsigned long timer = 10000;
const unsigned long buttonHoldTime = 3000;
// END User prefrences  --------------//

//--------------------------------//
// State managment
//--------------------------------//
bool startup = false;  // Add a flag to indicate initial blinking is done
bool isMelodyPlaying = false;
bool timerAlmostDone = false;
unsigned long timerStartTime = 0;
bool systemReset = false;  // Add a flag to indicate system reset
bool initialBlinkingDone = false;  // Add a flag to indicate initial blinking is done
// END State managment --------------//

const unsigned long breathCycle = 2000;

//--------------------------------//
// Setup
//--------------------------------//
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(vibrationPin, OUTPUT);
  pinMode(LED_PIN, OUTPUT); 
  digitalWrite(vibrationPin, LOW);
}
// END Setup  --------------//

//--------------------------------//
// Main script
//--------------------------------//
void loop() {
  // Update LED effects continuously
  updateLEDEffect();

  // Play melody if it's active
  playMelody();

  if(!startup) {
    startup = true;
    ledFlashing(200, 3);  // Initial 3 blinks
  }

  int buttonState = digitalRead(buttonPin);
  static unsigned long buttonPressStart = 0;
  static bool buttonHeld = false;
  
  // Print debug information
  printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);

  if (buttonState == LOW && timerStartTime == 0 && isMelodyPlaying) {
    if (!buttonHeld) {
      buttonPressStart = millis();
      buttonHeld = true;
    } else if (millis() - buttonPressStart > buttonHoldTime) {
      Serial.println("Button held down, stopping melody");
      isMelodyPlaying = false;
      timerAlmostDone = false;
      melodyIndex = 0;
      digitalWrite(vibrationPin, LOW);
      ledOff();  // Ensure LED is off
      buttonHeld = false;
      systemReset = true;
      startup = true;  // Reset startup state to enable breathing effect after reset
    }
  } else {
    buttonHeld = false;
  }

  // Check if button to start
  if (buttonState == LOW && timerStartTime == 0 && !isMelodyPlaying && !systemReset) {
    digitalWrite(vibrationPin, HIGH);
    delay(300);
    digitalWrite(vibrationPin, LOW);
    ledBreathing();
    timerStartTime = millis();
    Serial.println("Timer started");
    printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);
  }

  // Reset the system reset flag when the button is released
  if (buttonState == HIGH && systemReset) {
    systemReset = false;
  }

  // Play melody if timer is done
  if (timerStartTime != 0) {
    unsigned long elapsed = millis() - timerStartTime;

    if (elapsed >= timer) {
      // Timer is done start playing
      isMelodyPlaying = true;
      timerStartTime = 0;
      Serial.println("Timer elapsed. Playing melody...");
      ledFlashing(200);
      playMelody();
      printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);
    } else if (elapsed >= timer * 0.9) {
      // Timer almost done
      if (!timerAlmostDone) {
        timerAlmostDone = true;
        Serial.println("Timer almost done.");
      }
      int pulseDuration = map(elapsed, timer * 0.9, timer, 50, 500);
      int pauseDuration = map(elapsed, timer * 0.9, timer, 500, 50);

      ledStep();
    }
  }

  delay(100);
}
// END Main script  --------------//
