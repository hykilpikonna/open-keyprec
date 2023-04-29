//
// Created by Hykilpikonna on 4/21/23.
//

#include <Arduino.h>

#include "config.h"
#include "Adafruit_NeoPixel.h"
#include "Encoder.h"

/**
 * Class controlling the MIDI panel
 */
class Panel {
private:
    Adafruit_NeoPixel led_key;
    Adafruit_NeoPixel led_knob;
    Adafruit_NeoPixel led_rotary;

    u16 last_hue = 0;
    u8 brightness = 40;

    bool key_states[P_PINS_PER_MUX]{};
    bool btn_states[P_PINS_PER_MUX]{};
    u32 pot_states[P_PINS_PER_MUX]{};

    Encoder *encoders[P_NUM_ROTARY]{};
    int encoder_states[P_NUM_ROTARY]{};

    TaskHandle_t panelThread{};

public:
    Panel() :
        led_key(4, P_LED_KEY, NEO_GRB + NEO_KHZ800),
        led_knob(9, P_LED_KNOB, NEO_GRB + NEO_KHZ800),
        led_rotary(9, P_LED_ROTARY, NEO_GRB + NEO_KHZ800)
    {}

    void begin()
    {
        for (int pin: P_MUX_SEL_OUT) pinModeSafe(pin, OUTPUT);
        for (int pin: P_ROTARY_A) pinModeSafe(pin, INPUT);
        for (int pin: P_ROTARY_B) pinModeSafe(pin, INPUT);
        pinModeSafe(P_BUTTON_MUX_IN, INPUT);
        pinModeSafe(P_KEY_MUX_IN, INPUT);
        pinModeSafe(P_KNOB_MUX_IN, INPUT);
        pinModeSafe(P_LED_KEY, OUTPUT);
        pinModeSafe(P_LED_KNOB, OUTPUT);
        pinModeSafe(P_LED_ROTARY, OUTPUT);

        led_key.begin();
        led_knob.begin();
        led_rotary.begin();

        // Initialize encoders
        for (int i = 0; i < P_NUM_ROTARY; i++)
        {
            encoders[i] = new Encoder(P_ROTARY_A[i], P_ROTARY_B[i]);
        }

        xTaskCreate(loopPanel, "loopPanel", 4096, this, 1, &panelThread);
    }

private:
    void readPanel()
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
                led_rotary.setPixelColor(i, Adafruit_NeoPixel::ColorHSV(last_hue, 255, brightness));
                led_rotary.show();
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
            vTaskDelay(1);

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
            int pot = (int) round(analogRead(P_KNOB_MUX_IN) / 16.0);
            onPotRead(i, pot);

            // If the state is changed, call potentiometer callback
            if (ABS(pot_states[i] - pot) > 4)
            {
                pot_states[i] = pot;
                onPotChange(i, pot);
            }
        }

        delay(10);
        led_key.show();
        led_knob.show();
        led_rotary.show();
    }

    void onKey(int id, bool state)
    {
        // Check if it's one of the larger keys (the first 4)
        if (id < 4)
        {
            if (state)
            {
                // Set a random color for the key's LED
                led_key.setPixelColor(id, Adafruit_NeoPixel::ColorHSV(random(0, 65535), 255, brightness));
                led_key.show();
            }
            else
            {
                // Clear the key's LED
                led_key.setPixelColor(id, 0);
                led_key.show();
            }
        }

        // Key 5 = clear
        if (id == 4 && state)
        {
            led_key.clear();
            led_key.show();
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
        led_knob.setPixelColor(id, Adafruit_NeoPixel::ColorHSV(last_hue, 255, value));
        led_knob.show();
    }

    void onPotChange(int id, u8 value)
    {
//        Serial.printf("Potentiometer changed - id: %d, value: %d\r\n", id, value);
    }

    [[noreturn]] static void loopPanel(void* pvParameters)
    {
        auto* panel = (Panel*) pvParameters;
        while (true)
        {
            panel->readPanel();
        }
    }
};

