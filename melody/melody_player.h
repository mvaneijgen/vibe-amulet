#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>

extern bool isMelodyPlaying;
extern const int vibrationPin;

const unsigned int melody[] = { 125, 125, 125, 125, 125, 325, 250, 250, 250, 325, 125, 125, 125, 125, 125, 125 };

unsigned long lastMelodyUpdate = 0;
int melodyIndex = 0;
bool isVibrating = false;
bool waitingForNextSequence = false;
unsigned long sequenceStartTime = 0;

void playMelody() {
  if (!isMelodyPlaying) {
    digitalWrite(vibrationPin, LOW);
    return;
  }

  unsigned long currentMillis = millis();

  if (waitingForNextSequence) {
    if (currentMillis - sequenceStartTime >= 2000) {  // Wait 2 seconds between sequences
      waitingForNextSequence = false;
      melodyIndex = 0;
      isVibrating = false;
    }
    return;
  }

  if (melodyIndex >= 16) {
    waitingForNextSequence = true;
    sequenceStartTime = currentMillis;
    digitalWrite(vibrationPin, LOW);
    return;
  }

  if (currentMillis - lastMelodyUpdate >= melody[melodyIndex]) {
    lastMelodyUpdate = currentMillis;
    isVibrating = !isVibrating;
    digitalWrite(vibrationPin, isVibrating ? HIGH : LOW);
    melodyIndex++;
  }
}

#endif
