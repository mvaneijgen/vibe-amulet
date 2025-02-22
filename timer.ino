#include "melody/melody_player.h"

//--------------------------------//
// Pin Selection 
//--------------------------------//
const int buttonPin = 0;
const int vibrationPin = 13;
const int ledPin = 2;
// END Pin Selection  --------------//

//--------------------------------//
// User preferences  
//--------------------------------//
const int melodyPicked = 0;
const unsigned long timer = 60000;
const unsigned long buttonHoldTime = 3000;  // 3 seconds
// END User preferences  --------------//

//--------------------------------//
// State management 
//--------------------------------//
bool isMelodyPlaying = false;
bool timerAlmostDone = false;
unsigned long timerStartTime = 0;
unsigned long buttonPressStart = 0;
bool buttonHeld = false;
// END State management  --------------//

const unsigned long breathCycle = 2000;

//--------------------------------//
// Setup  
//--------------------------------//
void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(vibrationPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(vibrationPin, LOW);
  digitalWrite(ledPin, LOW);
  delay(2500); // wait a moment before starting
  
  // 5 times blink to indicate the device is ready
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  
  Serial.println("Device is ready.");
  Serial.flush();
}
// END Setup  --------------//

//--------------------------------//
// Main script  
//--------------------------------//
void loop() {
  int buttonState = digitalRead(buttonPin);

  // Log button press once when pressed and held
  if (buttonState == LOW && buttonPressStart == 0) {
    buttonPressStart = millis();
    Serial.println("Button pressed.");
  }

  // Check if the melody is playing
  if (isMelodyPlaying) {
    // Button is pressed, check if held for long enough to stop melody
    if (buttonState == LOW) {
      // Check if button has been held for the defined time
        for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
      if (millis() - buttonPressStart >= buttonHoldTime && !buttonHeld) {
        buttonHeld = true; // mark button as held
        isMelodyPlaying = false;
        Serial.println("Melody stopped by button hold!");
        delay(10000); // wait before state can change again
        Serial.println("State reset. Melody is stopped, ready to start again.");
      }
    }
    return;  // exit if melody is playing
  }

  // If the button is pressed and the timer hasn't started, start the timer
  if (buttonState == LOW && timerStartTime == 0 && !isMelodyPlaying) {
    timerStartTime = millis();
    Serial.println("Timer started");
    Serial.print("Button State: ");
    Serial.print(buttonState == LOW ? "Pressed" : "Released");
    Serial.print(" | Melody Playing: ");
    Serial.print(isMelodyPlaying ? "Yes" : "No");
    Serial.print(" | Timer Start: ");
    Serial.println(timerStartTime != 0 ? "Active" : "Inactive");
  }

  if (timerStartTime != 0) {
    unsigned long elapsed = millis() - timerStartTime;

    if (elapsed >= timer) {
      isMelodyPlaying = true;
      timerStartTime = 0;
      Serial.println("Timer elapsed: Playing melody...");
      playMelody(melodyPicked);
    } else if (elapsed >= timer * 0.9) {  
      if (!timerAlmostDone) {
        timerAlmostDone = true;
        Serial.println("Timer almost done.");
      }
      int pulseDuration = map(elapsed, timer * 0.9, timer, 50, 500);
      int pauseDuration = map(elapsed, timer * 0.9, timer, 500, 50);
      
      digitalWrite(vibrationPin, HIGH);
      delay(pulseDuration);
      digitalWrite(vibrationPin, LOW);
      delay(pauseDuration);
    }

    int ledBrightness = (exp(sin(millis() * (PI / breathCycle))) - 0.367879) * 108;
    analogWrite(ledPin, ledBrightness);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(10); // debounce delay
}
// END Main script  --------------//
