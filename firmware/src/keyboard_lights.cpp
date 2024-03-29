//
// Created by Hykilpikonna on 4/21/23.
//

#include <Arduino.h>
#include "config.h"
#include "utils.h"
#include "Adafruit_NeoPixel.h"
#include <unordered_map>
#include <queue>

const int ANIM_QUEUE_SIZE = 100;
const int FRAME_DELAY_MS = 20;

using namespace std;

class KeyboardLights
{
private:
    Adafruit_NeoPixel led_key;
    unordered_map<int, int> key_to_light;
    TaskHandle_t thread{};

    u32 animation[ANIM_QUEUE_SIZE][LK_NUM_LIGHTS]{};
    u32 last_frame[LK_NUM_LIGHTS]{};
    int anim_index = 0;

    static int wrap(int i) { return (i + LK_NUM_LIGHTS) % LK_NUM_LIGHTS; }

    void update()
    {
        // Render this frame
        auto frame = animation[anim_index];
        for (int i = 0; i < LK_NUM_LIGHTS; i++)
        {
            if (frame[i] == 0 && last_frame[i] == 0) continue;
            last_frame[i] = frame[i];
            led_key.setPixelColor(i, frame[i]);
        }
        led_key.show();

        // Clear the current frame
        for (int i = 0; i < LK_NUM_LIGHTS; i++) frame[i] = 0;
        anim_index = (anim_index + 1) % ANIM_QUEUE_SIZE;
    }

    [[noreturn]] static void loop(void *pv)
    {
        auto *lights = (KeyboardLights *) pv;

        while (true)
        {
            lights->update();
            vTaskDelay(FRAME_DELAY_MS / portTICK_PERIOD_MS);
        }
    }

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
        pinModeSafe(LK_PIN, OUTPUT);

        led_key.begin();
        xTaskCreate(loop, "loopLights", 4096, this, 1, &thread);
    }

    void hit(int key)
    {
        // 1. Calculate the starting index
        int start = key_to_light[key];

        Serial.printf("Key %d -> Light %d\n", key, start);

        // 2. Start animation

        // Fade out for one light
//        for (int i = 0; i < 50; i++)
//        {
//            animation[(anim_index + i) % ANIM_QUEUE_SIZE][start] = Adafruit_NeoPixel::ColorHSV(0, 255, (50 - i) * 2);
//        }

        // Fade out from the center to both sides
        int hue = random(0, 65535);
        for (int i = 0; i < 25; i++)
        {
            // Pick a random hue
            Serial.printf("Hue: %d\n", hue);
            int v = (25 - i) * 2;
            int s = 255;
            auto frame = animation[(anim_index + i) % ANIM_QUEUE_SIZE];

            frame[start] = Adafruit_NeoPixel::ColorHSV(hue, s, v);

            // Sides
            for (int j = 1; j <= min(i, 10); j++)
            {
                frame[(start + j) % LK_NUM_LIGHTS] = Adafruit_NeoPixel::ColorHSV(hue, s, MAX(v - j * 2, 0));
                frame[(start - j + LK_NUM_LIGHTS) % LK_NUM_LIGHTS] = Adafruit_NeoPixel::ColorHSV(hue, s, MAX(v - j * 2, 0));
            }
        }
    }
};