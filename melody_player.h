  #ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>

//--------------------------------//
// 🎶 Possible patterns
//--------------------------------//
const unsigned int nokiaSMS[] = {125, 125, 125, 125, 125, 325, 250, 250, 250, 325, 125, 125, 125, 125, 125, 125};
const unsigned int morseSOS[] = {150, 150, 150, 450, 450, 450, 150, 150, 150};
const unsigned int heartbeat[] = {200, 100, 200, 500};
const unsigned int shortBuzz[] = {100, 50, 100};
const unsigned int longBuzz[] = {500, 200, 500};
const unsigned int doubleTap[] = {150, 100, 150, 400};
const unsigned int alarmAlert[] = {300, 150, 300, 150, 300, 150, 300};
// END 🎶 Possible patterns --------------//

//--------------------------------//
// 🧮 State management
//--------------------------------//
extern bool isMelodyPlaying;
extern const int vibrationPin;
const unsigned int *currentMelody = nokiaSMS;
unsigned int currentMelodyLength = sizeof(nokiaSMS) / sizeof(nokiaSMS[0]);
unsigned long lastMelodyUpdate = 0;
int melodyIndex = 0;
bool waitingForNextSequence = false;
unsigned long sequenceStartTime = 0;
// END State management --------------//

struct Melody {
  const char *name;
  const unsigned int *pattern;
  unsigned int length;
};

const Melody melodies[] = {
    {"nokiaSMS", nokiaSMS, sizeof(nokiaSMS) / sizeof(nokiaSMS[0])},
    {"morseSOS", morseSOS, sizeof(morseSOS) / sizeof(morseSOS[0])},
    {"heartbeat", heartbeat, sizeof(heartbeat) / sizeof(heartbeat[0])},
    {"shortBuzz", shortBuzz, sizeof(shortBuzz) / sizeof(shortBuzz[0])},
    {"longBuzz", longBuzz, sizeof(longBuzz) / sizeof(longBuzz[0])},
    {"doubleTap", doubleTap, sizeof(doubleTap) / sizeof(doubleTap[0])},
    {"alarmAlert", alarmAlert, sizeof(alarmAlert) / sizeof(alarmAlert[0])}};

void selectMelody(const String &melodyName) {
  for (const Melody &melody : melodies) {
    if (melodyName == melody.name) {
      currentMelody = melody.pattern;
      currentMelodyLength = melody.length;
      melodyIndex = 0;
      waitingForNextSequence = false;
      lastMelodyUpdate = millis();
      return;
    }
  }
}

void playMelody() {
  if (!isMelodyPlaying) {
    digitalWrite(vibrationPin, LOW);
    return;
  }

  unsigned long currentMillis = millis();

  if (waitingForNextSequence) {
    if (currentMillis - sequenceStartTime >=
        2000) {  // Wait 2 seconds between sequences
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
