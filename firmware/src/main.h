//
// Created by Hykilpikonna on 4/13/23.
//

#ifndef FIRMWARE_MAIN_H
#define FIRMWARE_MAIN_H

#include <Arduino.h>
#include <chrono>
#include "macros.h"
#include "config.h"
#include "Adafruit_NeoPixel.h"
#include "Encoder.h"

/**
 * Called when the sensor value changes
 *
 * @param id Sensor index
 */
void on_sensor_update(int id, u64 time, u32 last, u32 current);

void onKey(int id, bool state);
void onBtn(int id, bool state);
void onPotRead(int id, u8 value);
void onPotChange(int id, u8 value);

int multisampleRead(int pin, int samples);

[[noreturn]] void loopPanel(void* pvParameters);
[[noreturn]] void loopKeyboard(void* pvParameters);

#endif //FIRMWARE_MAIN_H
