//
// Created by Hykilpikonna on 4/13/23.
//

#ifndef FIRMWARE_MAIN_H
#define FIRMWARE_MAIN_H

/**
 * Called when the sensor value changes
 *
 * @param id Sensor index
 */
void on_sensor_update(int id, u64 time, u32 last, u32 current);

#endif //FIRMWARE_MAIN_H
