// starwars.h
#ifndef STARWARS_H
#define STARWARS_H

#include "pitches.h"

namespace starwars {
  const int melody[] = {
    NOTE_A4, 500,  NOTE_A4,500,  NOTE_A4,500,  NOTE_F4,350,  NOTE_C5,150,
    // Add the rest of the Star Wars melody here...
  };

  const int tempo = 108;
}

#endif