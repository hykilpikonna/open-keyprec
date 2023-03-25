#ifndef FIRMWARE_MACROS_H
#define FIRMWARE_MACROS_H

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define timeMillis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define min(a, b) ((a) < (b) ? (a) : (b))
#define let auto
#define val const auto

#endif //FIRMWARE_MACROS_H
