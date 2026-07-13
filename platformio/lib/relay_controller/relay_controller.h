#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

/*
 * relay_controller — owns the relay that drives the water pump.
 * In simulation the relay/LED represents the pump. Handles the
 * active-LOW/HIGH polarity internally so callers just say on/off.
 */
namespace RelayController {
void begin();
void setPump(bool on);
bool isPumpOn();
}  // namespace RelayController

#endif  // RELAY_CONTROLLER_H
