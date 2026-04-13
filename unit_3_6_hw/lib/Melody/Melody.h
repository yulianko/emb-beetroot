#ifndef MELODY_H
#define MELODY_H

#include <cstddef>

struct MelodyNote {
    enum Note { REST = 0, C4 = 262, D4 = 294, E4 = 330, F4 = 349, G4 = 392, A4 = 440, B4 = 494, C5 = 523 };

    Note frequency;
    double durationSec;
};

struct Melody {
    const MelodyNote* notes;
    size_t size;
};

#endif
