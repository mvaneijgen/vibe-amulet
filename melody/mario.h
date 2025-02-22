// mario.h
#ifndef MARIO_H
#define MARIO_H

#include "pitches.h"

namespace mario {
  const int melody[] = {
    NOTE_E7, 8,  NOTE_E7,8,  REST,8,  NOTE_E7,8,  REST,8,  NOTE_C7,8,  NOTE_E7,8,
    // Add the rest of the Mario melody here...
  };

  const int tempo = 200;
}

#endif