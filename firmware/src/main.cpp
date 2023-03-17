#include <Arduino.h>
#include <chrono>
#include <cinttypes>

#define u64 uint64_t
#define timeMillis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define min(a, b) ((a) < (b) ? (a) : (b))

u64 start_time = 0;

const int NUM_SENSORS = 2;
int pins[] = {34, 39};   // GPIO34
int notes[] = {84, 86};  // C6, D6
int lasts[NUM_SENSORS];  // variable to store the value coming from the sensor
u64 last_hit_times[NUM_SENSORS];

int max_sensor = 4096;
int max_threshold = 700;
int active_threshold = 400;  // Minimum value to be considered as a hit

void setup()
{
    // Initialize pin and serial
    for (int pin : pins) pinMode(pin, INPUT);
    Serial.begin(9600);
    Serial.printf("Initialized\r\n");

    start_time = timeMillis();
}

/**
 * Called when the sensor value changes
 *
 * @param id Sensor index
 */
void on_sensor_update(int id, u64 time, int last, int current)
{
    // If the last value is larger than the current value, check timeout
    if (last > current && last > active_threshold) {
        u64 elapsed = time - last_hit_times[id];
        if (elapsed < 150) {
            // If the last hit is too close, ignore this hit
            return;
        }

        // The last value is a local maximum,
        // and we read it as the hit strength of our note. Send midi command to the host.
        // /hit <note> <velocity>
        Serial.printf("/hit %d %d\r\n", notes[id], min((last - active_threshold) * 127 / (max_threshold - active_threshold), 127));

        // Update last hit time
        last_hit_times[id] = time;
    }
}

void loop() {
    u64 time = timeMillis();

    // Read sensor value from each pin
    for (int i = 0; i < NUM_SENSORS; i++) {
        int v = analogRead(pins[i]);
        if (v != lasts[i]) {
            u64 elapsed = time - start_time;
            Serial.printf("Sensor #%d - %" PRIu64 ": %d\r\n", i, elapsed, v);

            on_sensor_update(i, time, lasts[i], v);
        }
        lasts[i] = v;
    }
}