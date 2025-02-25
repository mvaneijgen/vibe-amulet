#ifndef LED_EFFECTS_H
#define LED_EFFECTS_H

#include <Arduino.h>

#ifndef LED_PIN
#define LED_PIN 2
#endif

enum LEDEffect {
    OFF,
    BREATHING,
    STEP,
    FLASHING
};

LEDEffect currentEffect = OFF;
unsigned long lastUpdate = 0;
int brightness = 0;
int fadeAmount = 5;
bool ledState = false;
int flashInterval = 500;
int blinkCount = 0;
int blinkTimes = -1;  // -1 means blink indefinitely

void setLEDEffect(LEDEffect effect, int interval = 500, int times = -1) {
    if (currentEffect != effect) {
        currentEffect = effect;
        lastUpdate = millis();
        brightness = 0;
        fadeAmount = 5;
        blinkCount = 0;
        blinkTimes = times;
        if (effect == OFF) {
            digitalWrite(LED_PIN, LOW);
        } else if (effect == FLASHING) {
            flashInterval = interval;
        }
    } else if (effect == OFF) {
        digitalWrite(LED_PIN, LOW);
    }
}

void updateLEDEffect() {
    unsigned long currentMillis = millis();
    switch (currentEffect) {
        case BREATHING:
            if (currentMillis - lastUpdate > 30) {
                lastUpdate = currentMillis;
                brightness += fadeAmount;
                if (brightness <= 0 || brightness >= 255) {
                    fadeAmount = -fadeAmount;
                }
                analogWrite(LED_PIN, brightness);
            }
            break;
        case STEP:
            if (currentMillis - lastUpdate > 300) {
                lastUpdate = currentMillis;
                brightness += 51;
                if (brightness > 255) brightness = 0;
                analogWrite(LED_PIN, brightness);
            }
            break;
        case FLASHING:
            if (currentMillis - lastUpdate > flashInterval) {
                lastUpdate = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState ? HIGH : LOW);
                if (blinkTimes > 0) {
                    blinkCount++;
                    if (blinkCount >= blinkTimes * 2) {  // Each blink consists of two states (HIGH and LOW)
                        setLEDEffect(OFF);
                    }
                }
            }
            break;
        case OFF:
        default:
            digitalWrite(LED_PIN, LOW);
            break;
    }
}

void ledBreathing() {
    setLEDEffect(BREATHING);
}

void ledStep() {
    setLEDEffect(STEP);
}

void ledFlashing(int interval = 500, int times = -1) {
    setLEDEffect(FLASHING, interval, times);
}

void ledOff() {
    setLEDEffect(OFF);
}

#endif
