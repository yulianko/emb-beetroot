#ifndef MELODIES_H
#define MELODIES_H

#include "Melody.h"

static constexpr MelodyNote NEW_ATTEMPT_NOTES[] = {
    {MelodyNote::C4, 0.08},
    {MelodyNote::REST, 0.04},
    {MelodyNote::E4, 0.08},
    {MelodyNote::REST, 0.04},
    {MelodyNote::G4, 0.12},
    {MelodyNote::REST, 0.04},
    {MelodyNote::C5, 0.16},
};

static constexpr MelodyNote GRANTED_NOTES[] = {
    {MelodyNote::C4, 0.1},
    {MelodyNote::E4, 0.1},
    {MelodyNote::G4, 0.1},
    {MelodyNote::REST, 0.05},
    {MelodyNote::E4, 0.1},
    {MelodyNote::G4, 0.1},
    {MelodyNote::C5, 0.15},
    {MelodyNote::REST, 0.05},
    {MelodyNote::C5, 0.35},
};

static constexpr MelodyNote LOCKED_NOTES[] = {
    {MelodyNote::C5, 0.15},
    {MelodyNote::B4, 0.15},
    {MelodyNote::A4, 0.15},
    {MelodyNote::REST, 0.05},
    {MelodyNote::G4, 0.15},
    {MelodyNote::F4, 0.15},
    {MelodyNote::E4, 0.15},
    {MelodyNote::REST, 0.05},
    {MelodyNote::D4, 0.5},
};

static constexpr MelodyNote WRONG_CODE_NOTES[] = {
    {MelodyNote::G4, 0.1},
    {MelodyNote::REST, 0.04},
    {MelodyNote::E4, 0.1},
    {MelodyNote::REST, 0.04},
    {MelodyNote::C4, 0.1},
    {MelodyNote::REST, 0.04},
    {MelodyNote::C4, 0.25},
};

static constexpr Melody NEW_ATTEMPT = {NEW_ATTEMPT_NOTES, sizeof(NEW_ATTEMPT_NOTES) / sizeof(MelodyNote)};
static constexpr Melody GRANTED = {GRANTED_NOTES, sizeof(GRANTED_NOTES) / sizeof(MelodyNote)};
static constexpr Melody LOCKED = {LOCKED_NOTES, sizeof(LOCKED_NOTES) / sizeof(MelodyNote)};
static constexpr Melody WRONG_CODE = {WRONG_CODE_NOTES, sizeof(WRONG_CODE_NOTES) / sizeof(MelodyNote)};

#endif
