#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>

extern bool isMelodyPlaying;
extern const int vibrationPin;

// Define melodies
const unsigned int nokiaSMS[] = {125, 125, 125, 125, 125, 325, 250, 250,
                                 250, 325, 125, 125, 125, 125, 125, 125};
const unsigned int morseSOS[] = {150, 150, 150, 450, 450, 450, 150, 150, 150};
const unsigned int heartbeat[] = {200, 100, 200, 500};
const unsigned int shortBuzz[] = {100, 50, 100};
const unsigned int longBuzz[] = {500, 200, 500};
const unsigned int doubleTap[] = {150, 100, 150, 400};
const unsigned int alarmAlert[] = {300, 150, 300, 150, 300, 150, 300};

// Melody variables
const unsigned int *currentMelody = nokiaSMS;
unsigned int currentMelodyLength = sizeof(nokiaSMS) / sizeof(nokiaSMS[0]);
unsigned long lastMelodyUpdate = 0;
int melodyIndex = 0;
bool waitingForNextSequence = false;
unsigned long sequenceStartTime = 0;

void selectMelody(const String &melodyName) {
  if (melodyName == "nokiaSMS") {
    currentMelody = nokiaSMS;
    currentMelodyLength = sizeof(nokiaSMS) / sizeof(nokiaSMS[0]);
  } else if (melodyName == "morseSOS") {
    currentMelody = morseSOS;
    currentMelodyLength = sizeof(morseSOS) / sizeof(morseSOS[0]);
  } else if (melodyName == "heartbeat") {
    currentMelody = heartbeat;
    currentMelodyLength = sizeof(heartbeat) / sizeof(heartbeat[0]);
  } else if (melodyName == "shortBuzz") {
    currentMelody = shortBuzz;
    currentMelodyLength = sizeof(shortBuzz) / sizeof(shortBuzz[0]);
  } else if (melodyName == "longBuzz") {
    currentMelody = longBuzz;
    currentMelodyLength = sizeof(longBuzz) / sizeof(longBuzz[0]);
  } else if (melodyName == "doubleTap") {
    currentMelody = doubleTap;
    currentMelodyLength = sizeof(doubleTap) / sizeof(doubleTap[0]);
  } else if (melodyName == "alarmAlert") {
    currentMelody = alarmAlert;
    currentMelodyLength = sizeof(alarmAlert) / sizeof(alarmAlert[0]);
  }
  melodyIndex = 0;
  waitingForNextSequence = false;
  lastMelodyUpdate = millis();
}

void playMelody() {
  if (!isMelodyPlaying) {
    digitalWrite(vibrationPin, LOW);
    return;
  }

  unsigned long currentMillis = millis();

  if (waitingForNextSequence) {
    if (currentMillis - sequenceStartTime >=
        2000) { // Wait 2 seconds between sequences
      waitingForNextSequence = false;
      melodyIndex = 0;
    }
    return;
  }

  if (melodyIndex >= currentMelodyLength) {
    waitingForNextSequence = true;
    sequenceStartTime = currentMillis;
    digitalWrite(vibrationPin, LOW);
    return;
  }

  if (currentMillis - lastMelodyUpdate >= currentMelody[melodyIndex]) {
    lastMelodyUpdate = currentMillis;
    digitalWrite(vibrationPin, melodyIndex % 2 == 0 ? HIGH : LOW);
    melodyIndex++;
  }
}

#endif
