#ifndef MELODY_PLAYER_H
#define MELODY_PLAYER_H

#include <Arduino.h>
#include "pitches.h"
#include "tetris.h"
#include "mario.h"
#include "starwars.h"

extern bool isMelodyPlaying;
extern const int vibrationPin;

// Array of melodies
const int* melodies[] = { 
  tetris::melody, 
  mario::melody, 
  starwars::melody 
};

const int tempos[] = { 
  tetris::tempo, 
  mario::tempo, 
  starwars::tempo 
};

const int notes[] = { 
  sizeof(tetris::melody) / sizeof(tetris::melody[0]) / 2,
  sizeof(mario::melody) / sizeof(mario::melody[0]) / 2,
  sizeof(starwars::melody) / sizeof(starwars::melody[0]) / 2
};

void playMelody(int melodyIndex) {
  int wholenote = (60000 * 4) / tempos[melodyIndex];
  int divider = 0, noteDuration = 0;

  while (isMelodyPlaying) {
    for (int thisNote = 0; thisNote < notes[melodyIndex] * 2; thisNote += 2) {
      divider = melodies[melodyIndex][thisNote + 1];
      if (divider > 0) {
        noteDuration = wholenote / divider;
      } else if (divider < 0) {
        noteDuration = wholenote / abs(divider);
        noteDuration *= 1.5;
      }

      if (melodies[melodyIndex][thisNote] != REST) {
        digitalWrite(vibrationPin, HIGH);
      }
      delay(noteDuration * 0.9);
      digitalWrite(vibrationPin, LOW);
      delay(noteDuration * 0.1);
    }
  }
}

#endif
