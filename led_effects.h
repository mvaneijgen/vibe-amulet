#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>

#ifndef LED_PIN
#define LED_PIN 2
#endif

//--------------------------------//
// 🌈 LED Effects Enum
//--------------------------------//
enum LEDEffect {
  OFF,
  BREATHING,
  STEP,
  FLASHING
};
// END LED Effects Enum --------------//

//--------------------------------//
// 🧮 State management
//--------------------------------//
LEDEffect currentEffect = OFF;
unsigned long lastUpdate = 0;
int brightness = 0;
int fadeAmount = 5;
bool ledState = false;
int flashInterval = 500;
int blinkCount = 0;
int blinkTimes = -1;
// END State management --------------//

//--------------------------------//
// 📋 Function Declarations
//--------------------------------//
// Forward declarations after enum
void ledOff();
void setLEDEffect(LEDEffect effect, int interval = 500, int times = -1);
void ledBreathing();
void ledStep();
void ledFlashing(int interval = 500, int times = -1);
// END Function Declarations --------------//

//--------------------------------//
// ⚙️ Function Implementations
//--------------------------------//
void ledOff() {
  digitalWrite(LED_PIN, LOW);
  analogWrite(LED_PIN, 0);
  currentEffect = OFF;
  brightness = 0;
  ledState = false;
  blinkCount = 0;
  blinkTimes = -1;
  fadeAmount = 5;
  lastUpdate = millis();
}

void setLEDEffect(LEDEffect effect, int interval, int times) {
  if (currentEffect == effect && effect != FLASHING)
    return;

  currentEffect = effect;
  lastUpdate = millis();
  blinkTimes = times;
  blinkCount = 0;
  ledState = false;
  flashInterval = interval;

  // Reset LED state
  switch (effect) {
    case OFF:
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, LOW);
      analogWrite(LED_PIN, 0);
      brightness = 0;
      break;
    case FLASHING:
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, LOW);
      break;
    case BREATHING:
      pinMode(LED_PIN, OUTPUT);
      brightness = 0;
      fadeAmount = 5;
      analogWrite(LED_PIN, brightness);
      break;
    case STEP:
      pinMode(LED_PIN, OUTPUT);
      brightness = 0;
      analogWrite(LED_PIN, brightness);
      break;
  }
}

void updateLEDEffect() {
  unsigned long currentMillis = millis();

  switch (currentEffect) {
    case BREATHING:
      if (currentMillis - lastUpdate >= 30) {
        lastUpdate = currentMillis;
        analogWrite(LED_PIN, brightness);
        brightness += fadeAmount;

        if (brightness <= 0 || brightness >= 255) {
          fadeAmount = -fadeAmount;
        }

        brightness = constrain(brightness, 0, 255);
      }
      break;

    case STEP:
      if (currentMillis - lastUpdate >= 300) {
        lastUpdate = currentMillis;
        analogWrite(LED_PIN, brightness);
        brightness =
            (brightness + 51) % 256;
      }
      break;

    case FLASHING:
      if (currentMillis - lastUpdate >= flashInterval) {
        lastUpdate = currentMillis;
        ledState = !ledState;
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, ledState ? HIGH : LOW);

        if (blinkTimes > 0) {
          blinkCount++;
          if (blinkCount >= blinkTimes * 2) {
            setLEDEffect(OFF);
          }
        }
      }
      break;

    case OFF:
      pinMode(LED_PIN, OUTPUT);
      digitalWrite(LED_PIN, LOW);
      break;
  }
}

void ledBreathing() { setLEDEffect(BREATHING); }

void ledStep() { setLEDEffect(STEP); }

void ledFlashing(int interval, int times) {
  setLEDEffect(FLASHING, interval, times);
}
// END Function Implementations --------------//

#endif
