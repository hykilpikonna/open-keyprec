#include <Arduino.h>
#include "config.h"
#include "Adafruit_NeoPixel.h"
#include "main.h"
#include "utils.h"
#include "panel.cpp"

u32 lasts[NUM_NOTES];  // variable to store the value coming from the sensor
u64 last_hit_times[NUM_NOTES];

let max_sensor = 4096;
let max_threshold = 2000;
let active_threshold = 100;  // Minimum value to be considered as a hit

let led_refresh_on = false;

Adafruit_NeoPixel lk(LK_NUM_LIGHTS, LK_PIN, NEO_GRB + NEO_KHZ800);

Panel panel;

void setup()
{
    // Initialize pins
    pinModeSafe(LED_REFRESH, OUTPUT);
    pinModeSafe(LK_PIN, OUTPUT);
    for (int pin: MUX_IN) pinModeSafe(pin, INPUT);
    for (int pin: MUX_SEL_OUT) pinModeSafe(pin, OUTPUT);

    lk.begin();

    // Initialize serial
    Serial.begin(115200);
    Serial.printf("Initialized\r\n");

    panel.begin();
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

u64 fps_last_update = 0;
u32 fps_updates = 0;
const u32 fps_interval_ms = 1000;

void countFps(u64 time)
{
    // Report FPS every second
    fps_updates++;
    if (time - fps_last_update >= fps_interval_ms)
    {
        fps_last_update = time;
        double fps = 1.0 * fps_updates / fps_interval_ms * 1000;
        Serial.printf("FPS: %.2f\r\n", fps);
        fps_updates = 0;
    }
}

void readKeyboard()
{
    u64 time = millis();
    countFps(time);

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
                on_sensor_update(note_id, time, lasts[note_id], v);
            }
            lasts[note_id] = v;
        }
    }
}

void loop()
{
    readKeyboard();

//    for (int i = 0; i < LK_NUM_LIGHTS; i++)
//    {
//        lk.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue + i * hue_interval, 255, brightness));
//    }
}
