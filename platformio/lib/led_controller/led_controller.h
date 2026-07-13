#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

/*
 * led_controller — owns the two indicator LEDs.
 *   Alarm (red)   = fire.
 *   Normal (green) = healthy / no fire.
 * Only one is ever lit at a time.
 */
namespace LedController {
void begin();
void showAlarm();   // red on, green off
void showNormal();  // green on, red off
}  // namespace LedController

#endif  // LED_CONTROLLER_H
