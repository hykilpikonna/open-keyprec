//
// Created by Hykilpikonna on 4/21/23.
//

#include <Arduino.h>
#include "config.h"
#include "Adafruit_NeoPixel.h"

class KeyboardLights
{
private:
    Adafruit_NeoPixel led_key;

public:
    KeyboardLights() : led_key(4, LK_PIN, NEO_GRB + NEO_KHZ800) {}

    void begin()
    {
        led_key.begin();
    }

    /**
     * Convert a key index to a light index
     *
     * The light strip's spacing doesn't match the keyboard's spacing, so we need to convert.
     * Light strip info are defined by LK_LIGHTS_PER_METER and LK_NUM_LIGHTS.
     * Key spacing info are defined by LK_KEY_SPACING_MM and LK_KEY_LEN_MM.
     *
     * @param key Key index from 0 to 60
     * @return Light index
     */
    static int keyToLight(int key)
    {
        return (int) (key * LK_KEY_SPACING_MM / LK_KEY_LEN_MM * LK_LIGHTS_PER_METER);
    }

    void hit(int key)
    {
        // 1. Calculate the starting index
        int start = keyToLight(key);

        Serial.printf("Key %d -> Light %d\n", key, start);

        // 2. Set the color of the lights
        led_key.setPixelColor(start, Adafruit_NeoPixel::ColorHSV(0, 255, 255));
    }
};