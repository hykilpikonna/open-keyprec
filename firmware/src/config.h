#ifndef FIRMWARE_CONFIG_H
#define FIRMWARE_CONFIG_H

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
const float LK_NUM_METERS = 2;
const int LK_NUM_LIGHTS = (int) round(LK_LIGHTS_PER_METER * LK_NUM_METERS);

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

#endif //FIRMWARE_CONFIG_H