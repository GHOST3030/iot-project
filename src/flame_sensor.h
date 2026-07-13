#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H

// ============================================================
// flame_sensor — reads the flame sensor's digital output with
// debouncing so electrical noise cannot trigger a false alarm.
// ============================================================

// Configure the sensor pin. Call once from setup().
void flameSensorSetup();

// Poll the sensor. Call on every loop() pass with the current
// millis() value; it self-limits to SENSOR_POLL_INTERVAL_MS.
void flameSensorUpdate(unsigned long now);

// True once a fire reading has been stable for
// FLAME_DEBOUNCE_COUNT consecutive polls.
bool flameSensorFireDetected();

#endif // FLAME_SENSOR_H
