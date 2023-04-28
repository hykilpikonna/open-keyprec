//
// Created by Hykilpikonna on 4/21/23.
//

#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include "Adafruit_NeoPixel.h"
#include <unordered_map>

class KeyboardLights
{
private:
    Adafruit_NeoPixel led_key;
    std::unordered_map<int, int> key_to_light;

public:
    KeyboardLights() : led_key(LK_NUM_LIGHTS, LK_PIN, NEO_GRB + NEO_KHZ800)
    {
        // Initialize key to light map
        int regi = 0;
        for (int i = NUM_NOTES - 1; i >= 0; i--)
        {
            auto note = notes[i];
            auto ki = (float) regi;
            if (note.name[1] == '#') ki -= 0.5;

            // Calculate the length from the start of the keyboard to the key
            ki *= LK_KEY_SPACING_MM + LK_KEY_LEN_MM;
            ki += LK_KEY_LEN_MM / 2.0; // Center of the key

            // Calculate the index of the light at the same length position
            ki *= LK_LIGHTS_PER_MM;

            // Convert key index to mm, then to light index
            key_to_light[i] = (int) round(ki);

            // Increment the regular note index if it's not a sharp note
            if (note.name[1] != '#') regi++;
        }

    }

    void begin()
    {
        led_key.begin();
    }

    void hit(int key)
    {
        // 1. Calculate the starting index
        int start = key_to_light[key];

        Serial.printf("Key %d -> Light %d\n", key, start);

        // 2. Set the color of the lights
        led_key.setPixelColor(start, Adafruit_NeoPixel::ColorHSV(0, 255, 100));

        led_key.show();
    }
};