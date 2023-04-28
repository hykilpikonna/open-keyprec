#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

#include "utils.h"

// ========================================
// Main Keyboard Pin Configuration
// ========================================

// LED indicator for pulling update
const int LED_REFRESH = 45;

// 4 1:16 Multiplexers: GPIO pins for each analog multiplexer that handles 12 sensors of an octave
// Notes are connected in order: Mux #1 (0-11), Mux #2 (12-23), Mux #3 (24-35), Mux #4 (36-47), Mux #5 (48-59)
const int NUM_MUX = 5;
const int PINS_PER_MUX = 12;
const int MUX_IN[NUM_MUX] = {4, 5, 6, 7, 8};

// Select pins for every multiplexer, each multiplexer has 4 select pins, all sel0 are connected to 14, etc.
const int NUM_MUX_SEL = 4;
const int MUX_SEL_OUT[NUM_MUX_SEL] = {14, 13, 12, 11};

// LED Light strip
const int LK_PIN = 2;
const int LK_LIGHTS_PER_METER = 60;
constexpr float LK_NUM_METERS = 2;
const int LK_KEY_SPACING_MM = 7;
const int LK_KEY_LEN_MM = 30;
const int LK_OFFSET_MM = 25; // Length in mm from the start of the strip to the first key
const float LK_LIGHTS_PER_MM = LK_LIGHTS_PER_METER / 1000.0;
constexpr int LK_NUM_LIGHTS = (int) (LK_LIGHTS_PER_METER * LK_NUM_METERS + 0.5);

// ========================================
// MIDI Panel Pin Configuration
// ========================================

// 3 1:8 Multiplexers for the midi panel
const int P_PINS_PER_MUX = 8;
const int P_KEY_MUX_IN = 36;
const int P_BUTTON_MUX_IN = 35;  // Digital signal inputs for button multiplexers
const int P_KNOB_MUX_IN = 10;  // Analog signal inputs for potentiometer

// Select pins for every multiplexer, each multiplexer has 3 select pins
const int P_NUM_MUX_SEL = 3;
const int P_MUX_SEL_OUT[P_NUM_MUX_SEL] = {37, 38, 39};

// Rotary encoder pins
const int P_NUM_ROTARY = 4;
const int P_ROTARY_A[P_NUM_ROTARY] = {18, 17, 16, 15};
const int P_ROTARY_B[P_NUM_ROTARY] = {19, 20, 21, 47};

// LED light strips
const int P_LED_KEY = 42;
const int P_LED_KNOB = 41;
const int P_LED_ROTARY = 40;

// ========================================
// Constants
// ========================================

typedef struct note
{
    char name[4];
    u16 midi;
} Note;

// 61 Notes from C2 to C7
const int NUM_NOTES = 61;
const int NUM_SHARP_NOTES = 25;
const int NUM_REGULAR_NOTES = NUM_NOTES - NUM_SHARP_NOTES;

const Note notes[] = {
        {"C2", 36}, {"C#2", 37}, {"D2", 38}, {"D#2", 39}, {"E2", 40}, {"F2", 41}, {"F#2", 42}, {"G2", 43}, {"G#2", 44}, {"A2", 45}, {"A#2", 46}, {"B2", 47},
        {"C3", 48}, {"C#3", 49}, {"D3", 50}, {"D#3", 51}, {"E3", 52}, {"F3", 53}, {"F#3", 54}, {"G3", 55}, {"G#3", 56}, {"A3", 57}, {"A#3", 58}, {"B3", 59},
        {"C4", 60}, {"C#4", 61}, {"D4", 62}, {"D#4", 63}, {"E4", 64}, {"F4", 65}, {"F#4", 66}, {"G4", 67}, {"G#4", 68}, {"A4", 69}, {"A#4", 70}, {"B4", 71},
        {"C5", 72}, {"C#5", 73}, {"D5", 74}, {"D#5", 75}, {"E5", 76}, {"F5", 77}, {"F#5", 78}, {"G5", 79}, {"G#5", 80}, {"A5", 81}, {"A#5", 82}, {"B5", 83},
        {"C6", 84}, {"C#6", 85}, {"D6", 86}, {"D#6", 87}, {"E6", 88}, {"F6", 89}, {"F#6", 90}, {"G6", 91}, {"G#6", 92}, {"A6", 93}, {"A#6", 94}, {"B6", 95},
        {"C7", 96}
};

#endif //FIRMWARE_CONFIG_H