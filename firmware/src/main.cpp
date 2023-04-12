#include <Arduino.h>
#include <chrono>
#include "macros.h"
#include "config.h"
#include "Adafruit_NeoPixel.h"
#include "Encoder.h"

// ========================================
// Code
// ========================================

u64 start_time = 0;
u64 last_refresh_time = 0;

typedef struct note
{
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

let max_sensor = 4096;
let max_threshold = 2000;
let active_threshold = 100;  // Minimum value to be considered as a hit

let led_refresh_on = false;

void pinModeSafe(int pin, int mode)
{
    if (pin == -1) return;
    pinMode(pin, mode);
}

Adafruit_NeoPixel p_led_key(4, P_LED_KEY, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel p_led_knob(9, P_LED_KNOB, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel p_led_rotary(9, P_LED_ROTARY, NEO_GRB + NEO_KHZ800);

u64 fps_time_counter = 0;
u32 fps_updates = 0;
u32 fps_interval_ms = 1000;

u16 last_hue = 0;
u8 brightness = 40;

bool key_states[P_PINS_PER_MUX];
bool btn_states[P_PINS_PER_MUX];
u32 pot_states[P_PINS_PER_MUX];

Encoder *encoders[P_NUM_ROTARY];
int encoder_states[P_NUM_ROTARY];

void setup()
{
    // Initialize pins
    pinModeSafe(LED_REFRESH, OUTPUT);
    for (int pin: MUX_IN) pinModeSafe(pin, INPUT);
    for (int pin: MUX_SEL_OUT) pinModeSafe(pin, OUTPUT);
    for (int pin: P_MUX_SEL_OUT) pinModeSafe(pin, OUTPUT);
    for (int pin: P_ROTARY_A) pinModeSafe(pin, INPUT);
    for (int pin: P_ROTARY_B) pinModeSafe(pin, INPUT);
    pinModeSafe(P_BUTTON_MUX_IN, INPUT);
    pinModeSafe(P_KEY_MUX_IN, INPUT);
    pinModeSafe(P_KNOB_MUX_IN, INPUT);
    pinModeSafe(P_LED_KEY, OUTPUT);
    pinModeSafe(P_LED_KNOB, OUTPUT);
    pinModeSafe(P_LED_ROTARY, OUTPUT);

    // Initialize encoders
    for (int i = 0; i < P_NUM_ROTARY; i++)
    {
        encoders[i] = new Encoder(P_ROTARY_A[i], P_ROTARY_B[i]);
    }

    // Initialize serial
    Serial.begin(9600);
    Serial.printf("Initialized\r\n");

    start_time = timeMillis();

    p_led_key.begin();
    p_led_knob.begin();
    p_led_rotary.begin();
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

void onKey(int id, bool state)
{
    // Check if it's one of the larger keys (the first 4)
    if (id < 4)
    {
        if (state)
        {
            // Set a random color for the key's LED
            p_led_key.setPixelColor(id, Adafruit_NeoPixel::ColorHSV(random(0, 65535), 255, brightness));
            p_led_key.show();
        }
        else
        {
            // Clear the key's LED
            p_led_key.setPixelColor(id, 0);
            p_led_key.show();
        }
    }

    // Key 5 = clear
    if (id == 4 && state)
    {
        p_led_key.clear();
        p_led_key.show();
    }

    Serial.printf("Key changed - id: %d, state: %d\r\n", id, state);
}

void onBtn(int id, bool state)
{
    Serial.printf("Button changed - id: %d, state: %d\r\n", id, state);
}

void onPotRead(int id, u8 value)
{
    // Set LED
    p_led_knob.setPixelColor(id, Adafruit_NeoPixel::ColorHSV(last_hue, 255, value));
    p_led_knob.show();
}

void onPotChange(int id, u8 value)
{
    Serial.printf("Potentiometer changed - id: %d, value: %d\r\n", id, value);
}

int multisampleRead(int pin, int samples)
{
    int sum = 0;
    for (int i = 0; i < samples; ++i)
    {
        sum += analogRead(pin);
    }
    return (int) round(((double) sum) / samples);
}

void loopPanel()
{
    const auto hue_interval = 512;
    last_hue += hue_interval;

    // Read rotary encoders
    for (int i = 0; i < P_NUM_ROTARY; ++i)
    {
        int state = encoders[i]->read();
        if (encoder_states[i] != state)
        {
            encoder_states[i] = state;
            Serial.printf("Rotary changed - id: %d, value: %d\r\n", i, state);
            p_led_rotary.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue, 255, brightness));
            p_led_rotary.show();
        }
    }

    // Read buttons
    for (int i = 0; i < P_PINS_PER_MUX; ++i)
    {
        // Set select pins
        for (int j = 0; j < P_NUM_MUX_SEL; ++j)
        {
            // i >> j is the jth bit of i
            digitalWrite(P_MUX_SEL_OUT[j], (i >> j) & 1);
        }
        delay(1);

        // Read button
        int key = !digitalRead(P_KEY_MUX_IN);
        int btn = !digitalRead(P_BUTTON_MUX_IN);

        // If the state is changed, call button callback
        if (key_states[i] != key)
        {
            key_states[i] = key;
            onKey(i, key);
        }

        if (btn_states[i] != btn)
        {
            btn_states[i] = btn;
            onBtn(i, btn);
        }

        // Read potentiometer
        int pot = (int) round(multisampleRead(P_KNOB_MUX_IN, 2) / 16.0);
        onPotRead(i, pot);

        // If the state is changed, call potentiometer callback
        if (abs(pot_states[i] - pot) > 4)
        {
            pot_states[i] = pot;
            onPotChange(i, pot);
        }
    }

//    for (int i = 0; i < 9; i++)
//    {
//        p_led_key.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue + i * hue_interval, 255, brightness));
//        p_led_knob.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue + 16384 + i * hue_interval, 255, brightness));
//        p_led_rotary.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue + 32768 + i * hue_interval, 255, brightness));
//    }

    delay(10);
    p_led_key.show();
    p_led_knob.show();
    p_led_rotary.show();
}

void loopKeyboard()
{
    u64 time = timeMillis();
    u64 elapsed = time - last_refresh_time;
    last_refresh_time = time;

    // Report FPS every second
    fps_time_counter += elapsed;
    fps_updates++;
    if (fps_time_counter >= fps_interval_ms)
    {
        fps_time_counter -= fps_interval_ms;
        double fps = 1.0 * fps_updates / fps_interval_ms * 1000;
        Serial.printf("FPS: %.2f\r\n", fps);
        fps_updates = 0;
    }

    // Toggle LED refresh indicator
    digitalWrite(LED_REFRESH, led_refresh_on = !led_refresh_on);

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
                // Serial.printf("%s %d\r\n", notes[note_id].name, v);
                on_sensor_update(note_id, time, lasts[note_id], v);
            }
            lasts[note_id] = v;
        }
    }
}

void loop()
{
    loopKeyboard();
    loopPanel();
}
