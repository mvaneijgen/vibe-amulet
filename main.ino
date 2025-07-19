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
unsigned long buttonPressStart =
    0;                   // Remove static to make it accessible to debug.h
bool buttonHeld = false; // Remove static to make it accessible to debug.h
unsigned long restartDelayStart = 0; // Track restart delay timing
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

  // Test motor briefly
  Serial.println("Testing motor...");
  digitalWrite(vibrationPin, HIGH);
  delay(500);
  digitalWrite(vibrationPin, LOW);
  Serial.println("Motor test complete");
}
// END Setup  --------------//

//--------------------------------//
// ⏱️ Start timer
//--------------------------------//
void startTimer(int buttonState) {
  // Button is open by default (HIGH), closes when pressed (LOW)
  if (buttonState == LOW && timerStartTime == 0 && !isMelodyPlaying) {
    // Check if restart delay is active
    if (millis() - restartDelayStart < 3000) { // 3 second delay
      return; // Don't start timer during restart delay
    }

    // Clear reset state on first button press
    if (systemReset) {
      systemReset = false;
      Serial.println("System reset cleared - ready for timer");
      return; // Don't start timer on this press, just clear reset
    }

    timerStartTime = millis();
    Serial.println("Timer started - 10 second countdown begins");
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
  timerAlmostDone = false; // Clear the almost done state
  currentEffect = OFF;
  ledFlashing(200, -1);
  Serial.println("Melody should now be playing continuously!");
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
  if (timerStartTime != 0 &&
      !isMelodyPlaying) { // Only run if melody isn't already playing
    unsigned long elapsed = millis() - timerStartTime; // Fixed calculation
    unsigned long remaining = timer - elapsed;

    // Debug timer values
    static unsigned long lastTimerDebug = 0;
    if (millis() - lastTimerDebug > 2000) { // Every 2 seconds
      lastTimerDebug = millis();
      Serial.print("DEBUG: elapsed=");
      Serial.print(elapsed);
      Serial.print(", remaining=");
      Serial.print(remaining);
      Serial.print(", timer=");
      Serial.print(timer);
      Serial.print(", timerStartTime=");
      Serial.println(timerStartTime);
    }

    if (remaining <= 0) {
      Serial.println(
          "DEBUG: Timer should be done! Calling handleTimerElapsed()");
      handleTimerElapsed();
      return; // Exit immediately after starting melody
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
  if (buttonState == LOW && isMelodyPlaying) {
    if (!buttonHeld) {
      buttonPressStart = millis();
      buttonHeld = true;
      Serial.println("Button pressed - start holding to stop melody");
    } else if (millis() - buttonPressStart > buttonHoldTime) {
      Serial.println("Button held down, stopping melody");
      isMelodyPlaying = false;
      timerAlmostDone = false;
      buttonHeld = false;
      systemReset = true;
      digitalWrite(vibrationPin, LOW);
      ledOff();

      // Add restart delay - system won't respond for 3 seconds
      restartDelayStart = millis();
      Serial.println("System disabled for 3 seconds - please wait...");
    }
  } else {
    if (buttonHeld) {
      Serial.println("Button released before hold time");
    }
    buttonHeld = false;
  }

  // Manual test: Double-press to force melody start (for debugging)
  static bool lastButtonState = HIGH;
  static unsigned long lastPressTime = 0;

  // Check if restart delay is active
  if (millis() - restartDelayStart < 3000) { // 3 second delay
    return; // Don't process button presses during restart delay
  }

  if (buttonState == LOW && lastButtonState == HIGH) {
    unsigned long currentTime = millis();
    if (currentTime - lastPressTime < 1000) { // Double press within 1 second
      Serial.println("Double press detected - forcing melody start!");
      isMelodyPlaying = true;
      timerStartTime = 0;
      systemReset = false;
      ledFlashing(200, -1);
    }
    lastPressTime = currentTime;
  }
  lastButtonState = buttonState;
}
// END 🧹 Reset state --------------//

//--------------------------------//
// Main loop
//--------------------------------//
void loop() {
  updateLEDEffect();
  playMelody();
  int buttonState = digitalRead(buttonPin);

  // Debug button state more frequently
  static unsigned long lastButtonDebug = 0;
  if (millis() - lastButtonDebug > 1000) { // Every second
    lastButtonDebug = millis();
    Serial.print("Button state: ");
    Serial.print(buttonState == LOW ? "PRESSED" : "RELEASED");
    Serial.print(" | Timer active: ");
    Serial.print(timerStartTime != 0 ? "YES" : "NO");
    Serial.print(" | Melody playing: ");
    Serial.print(isMelodyPlaying ? "YES" : "NO");
    Serial.print(" | Time remaining: ");
    if (timerStartTime != 0) {
      unsigned long elapsed = millis() - timerStartTime;
      unsigned long remaining = timer - elapsed;
      Serial.print(remaining / 1000);
      Serial.print("s");
    } else {
      Serial.print("N/A");
    }
    Serial.println();
  }

  // Emergency override: if timer has been running for more than 12 seconds,
  // force melody
  if (timerStartTime != 0 && !isMelodyPlaying) {
    unsigned long elapsed = millis() - timerStartTime;
    if (elapsed > 12000) { // 12 seconds (2 seconds extra)
      Serial.println("EMERGENCY: Timer stuck, forcing melody start!");
      handleTimerElapsed();
    }
  }

  // Show restart delay countdown
  if (millis() - restartDelayStart < 3000) {
    unsigned long remainingDelay = 3000 - (millis() - restartDelayStart);
    static unsigned long lastDelayDebug = 0;
    if (millis() - lastDelayDebug > 1000) { // Every second
      lastDelayDebug = millis();
      Serial.print("Restart delay: ");
      Serial.print(remainingDelay / 1000);
      Serial.println(" seconds remaining");
    }
  }

  printDebugInfo(buttonState, isMelodyPlaying, timerStartTime);
  resetState(buttonState);
  startTimer(buttonState);
  playMelodyIfTimerDone();
  delay(100);
}
// END Main loop --------------//
