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

  // Indicate ESP32 is ready with blink sequence
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);

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

  static unsigned long lastVibrationUpdate = 0;
  static bool isVibrating = false;

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
    unsigned long elapsed = millis();
    unsigned long remaining = timer - (elapsed - timerStartTime);

    if (remaining <= 0) {
      // Timer is done, start playing melody
      isMelodyPlaying = true;
      timerStartTime = 0;
      Serial.println("Timer elapsed. Playing melody...");
      
      // Force LED effect reset and start flashing
      ledOff();
      delay(10);  // Small delay to ensure the LED state is reset
      ledFlashing(200, -1);  // -1 means continuous flashing
      
      printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);
    } else if (remaining <= timer * 0.1) {  // Last 10% of timer
      // Timer almost done
      if (!timerAlmostDone) {
        timerAlmostDone = true;
        Serial.println("Timer almost done.");
        ledStep();
      }

      // Calculate pulse and pause durations
      int pulseDuration = map(remaining, timer * 0.1, 0, 50, 500);
      int pauseDuration = map(remaining, timer * 0.1, 0, 500, 50);
      
      // Non-blocking vibration pattern
      unsigned long currentMillis = millis();
      if (isVibrating && currentMillis - lastVibrationUpdate >= pulseDuration) {
        digitalWrite(vibrationPin, LOW);
        isVibrating = false;
        lastVibrationUpdate = currentMillis;
      } else if (!isVibrating && currentMillis - lastVibrationUpdate >= pauseDuration) {
        digitalWrite(vibrationPin, HIGH);
        isVibrating = true;
        lastVibrationUpdate = currentMillis;
      }
    }
  }

  delay(100);
}
// END Main script  --------------//
