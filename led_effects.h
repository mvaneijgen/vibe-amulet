#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>
#include <FastLED.h>

#ifndef LED_PIN
#define LED_PIN 27 // WS2812B data pin
#endif

#define NUM_LEDS 1     // Number of LEDs in your strip
#define BRIGHTNESS 255 // Maximum brightness (0-255)

//--------------------------------//
// 🌈 LED Effects Enum
//--------------------------------//
enum LEDEffect { OFF, BREATHING, STEP, FLASHING };
// END LED Effects Enum --------------//

//--------------------------------//
// 🧮 State management
//--------------------------------//
CRGB leds[NUM_LEDS]; // FastLED array
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
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
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
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    brightness = 0;
    break;
  case FLASHING:
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    break;
  case BREATHING:
    brightness = 0;
    fadeAmount = 5;
    fill_solid(leds, NUM_LEDS, CRGB::White);
    leds[0].nscale8(brightness);
    FastLED.show();
    break;
  case STEP:
    brightness = 0;
    fill_solid(leds, NUM_LEDS, CRGB::White);
    leds[0].nscale8(brightness);
    FastLED.show();
    break;
  }
}

void updateLEDEffect() {
  unsigned long currentMillis = millis();

  switch (currentEffect) {
  case BREATHING:
    if (currentMillis - lastUpdate >= 30) {
      lastUpdate = currentMillis;
      fill_solid(leds, NUM_LEDS, CRGB::White);
      leds[0].nscale8(brightness);
      FastLED.show();
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
      fill_solid(leds, NUM_LEDS, CRGB::White);
      leds[0].nscale8(brightness);
      FastLED.show();
      brightness = (brightness + 51) % 256;
    }
    break;

  case FLASHING:
    if (currentMillis - lastUpdate >= flashInterval) {
      lastUpdate = currentMillis;
      ledState = !ledState;
      fill_solid(leds, NUM_LEDS, ledState ? CRGB::White : CRGB::Black);
      FastLED.show();

      if (blinkTimes > 0) {
        blinkCount++;
        if (blinkCount >= blinkTimes * 2) {
          setLEDEffect(OFF);
        }
      }
    }
    break;

  case OFF:
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
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
