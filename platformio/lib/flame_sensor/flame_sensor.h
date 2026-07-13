#ifndef FLAME_SENSOR_H
#define FLAME_SENSOR_H

/*
 * flame_sensor — reads the IR flame sensor.
 * In Wokwi it is simulated by a pushbutton (pressed = flame),
 * so we enable the internal pull-up. On real hardware the module's
 * digital output drives the line; the reading logic is unchanged.
 */
namespace FlameSensor {
void begin();
bool isFlameDetected();
}  // namespace FlameSensor

#endif  // FLAME_SENSOR_H
