#include <Arduino.h>
#include <chrono>
#include "macros.h"
#include "config.h"

// ========================================
// Code
// ========================================

u64 start_time = 0;

typedef struct note {
    char name[4];
    u16 midi;
} Note;

// 61 Notes from C2 to C7
const int NUM_NOTES = 61;
const Note notes[] = {
    {"C2", 36}, {"C#2", 37}, {"D2", 38}, {"D#2", 39}, {"E2", 40}, {"F2", 41}, {"F#2", 42}, {"G2", 43}, {"G#2", 44}, {"A2", 45}, {"A#2", 46}, {"B2", 47},
    {"C3", 48}, {"C#3", 49}, {"D3", 50}, {"D#3", 51}, {"E3", 52}, {"F3", 53}, {"F#3", 54}, {"G3", 55}, {"G#3", 56}, {"A3", 57}, {"A#3", 58}, {"B3", 59},
    {"C4", 60}, {"C#4", 61}, {"D4", 62}, {"D#4", 63}, {"E4", 64}, {"F4", 65}, {"F#4", 66}, {"G4", 67}, {"G#4", 68}, {"A4", 69}, {"A#4", 70}, {"B4", 71},
    {"C5", 72}, {"C#5", 73}, {"D5", 74}, {"D#5", 75}, {"E5", 76}, {"F5", 77}, {"F#5", 78}, {"G5", 79}, {"G#5", 80}, {"A5", 81}, {"A#5", 82}, {"B5", 83},
    {"C6", 84}, {"C#6", 85}, {"D6", 86}, {"D#6", 87}, {"E6", 88}, {"F6", 89}, {"F#6", 90}, {"G6", 91}, {"G#6", 92}, {"A6", 93}, {"A#6", 94}, {"B6", 95},
    {"C7", 96}
};

u32 lasts[NUM_NOTES];  // variable to store the value coming from the sensor
u64 last_hit_times[NUM_NOTES];

val max_sensor = 4096;
val max_threshold = 2000;
val active_threshold = 100;  // Minimum value to be considered as a hit

let led_refresh_on = false;

void setup()
{

    // Initialize pin and serial
    for (int pin: MUX_IN) pinMode(pin, INPUT);
    for (int pin: MUX_SEL_OUT) pinMode(pin, OUTPUT);
    Serial.begin(9600);
    Serial.printf("Initialized\r\n");

    start_time = timeMillis();
}

/**
 * Called when the sensor value changes
 *
 * @param id Sensor index
 */
void on_sensor_update(int id, u64 time, u32 last, u32 current)
{
    // If the last value is larger than the current value, check timeout
    if (last > current && last > active_threshold)
    {
        u64 elapsed = time - last_hit_times[id];
        if (elapsed < 150)
        {
            // If the last hit is too close, ignore this hit
            return;
        }

        // The last value is a local maximum,
        // and we read it as the hit strength of our note. Send midi command to the host.
        // /hit <note> <velocity>
        Serial.printf("/hit %d %d\r\n", notes[id].midi,
                      min((last - active_threshold) * 127 / (max_threshold - active_threshold), 127));

        // Update last hit time
        last_hit_times[id] = time;
    }
}

void loop()
{
    u64 time = timeMillis();
    u64 elapsed = time - start_time;

    // Toggle LED refresh indicator
    digitalWrite(LED_REFRESH, led_refresh_on = !led_refresh_on);
//    Serial.printf("%" PRIu64 "=============\r\n", elapsed);

    // Loop through each multiplexer state
    for (int i = 0; i < PINS_PER_MUX; i++)
    {
        // Set select pins
        for (int j = 0; j < NUM_MUX_SEL; j++)
        {
            // i >> j is the jth bit of i
            digitalWrite(MUX_SEL_OUT[j], (i >> j) & 1);
        }

        // Read four input pins from the multiplexer
        for (int j = 0; j < NUM_MUX; j++)
        {
            int note_id = j * PINS_PER_MUX + i;
            if (note_id >= NUM_NOTES) break;

            // Read the analog input
            u32 v = analogRead(MUX_IN[j]);
            if (v != lasts[note_id])
            {
                // Serial prints are really slow, so don't use them in debug mode
                Serial.printf("%s %d\r\n", notes[note_id].name, v);
                on_sensor_update(note_id, time, lasts[note_id], v);
            }
            lasts[note_id] = v;
        }
    }
//    Serial.printf("Loop %" PRIu64 "\r\n", elapsed);
}