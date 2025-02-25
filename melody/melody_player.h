#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>

extern bool isMelodyPlaying;
extern const int vibrationPin;

const unsigned int melody[] = { 125, 125, 125, 125, 125, 325, 250, 250, 250, 325, 125, 125, 125, 125, 125, 125 };

unsigned long lastMelodyUpdate = 0;
int melodyIndex = 0;
bool melodyDelay = false;

void playMelody() {
  if (isMelodyPlaying) {
    unsigned long currentMillis = millis();
    if (melodyDelay) {
      if (currentMillis - lastMelodyUpdate >= 1000) {  // 1 second delay between repetitions
        melodyDelay = false;
        melodyIndex = 0;
        lastMelodyUpdate = currentMillis;
      }
    } else {
      if (currentMillis - lastMelodyUpdate >= melody[melodyIndex]) {
        lastMelodyUpdate = currentMillis;
        if (melodyIndex % 2 == 0) digitalWrite(vibrationPin, HIGH);
        else digitalWrite(vibrationPin, LOW);
        melodyIndex++;
        if (melodyIndex >= 16) {
          melodyDelay = true;
          digitalWrite(vibrationPin, LOW);
        }
      }
    }
  }
}

#endif
