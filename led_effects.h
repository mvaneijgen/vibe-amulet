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
    currentEffect = effect;
    lastUpdate = millis();
    blinkTimes = times;
    blinkCount = 0;
    ledState = false;
    
    if (effect == OFF) {
        digitalWrite(LED_PIN, LOW);
        analogWrite(LED_PIN, 0);
        brightness = 0;
        fadeAmount = 5;
    } else if (effect == FLASHING) {
        flashInterval = interval;
        ledState = false;
    } else if (effect == BREATHING) {
        brightness = 0;
        fadeAmount = 5;
    } else if (effect == STEP) {
        brightness = 0;
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
                
                // Ensure brightness stays within bounds
                brightness = constrain(brightness, 0, 255);
            }
            break;
            
        case STEP:
            if (currentMillis - lastUpdate >= 300) {
                lastUpdate = currentMillis;
                analogWrite(LED_PIN, brightness);
                brightness = (brightness + 51) % 256;  // Increment by 51 (255/5) for 5 steps
            }
            break;
            
        case FLASHING:
            if (currentMillis - lastUpdate >= flashInterval) {
                lastUpdate = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState ? HIGH : LOW);
                
                if (blinkTimes > 0) {
                    blinkCount++;
                    if (blinkCount >= blinkTimes * 2) {
                        currentEffect = OFF;
                        digitalWrite(LED_PIN, LOW);
                    }
                }
            }
            break;
            
        case OFF:
            digitalWrite(LED_PIN, LOW);
            analogWrite(LED_PIN, 0);
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
    // Force LED off immediately
    digitalWrite(LED_PIN, LOW);
    analogWrite(LED_PIN, 0);
    
    // Reset all LED-related variables
    currentEffect = OFF;
    brightness = 0;
    ledState = false;
    blinkCount = 0;
    blinkTimes = -1;
    fadeAmount = 5;
    lastUpdate = millis();
}

#endif
