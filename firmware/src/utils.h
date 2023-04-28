#ifndef FIRMWARE_UTILS_H
#define FIRMWARE_UTILS_H

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))
#define let auto

#define pinModeSafe(pin, mode) do { if (pin != -1) pinMode(pin, mode); } while (false)

#endif //FIRMWARE_UTILS_H
