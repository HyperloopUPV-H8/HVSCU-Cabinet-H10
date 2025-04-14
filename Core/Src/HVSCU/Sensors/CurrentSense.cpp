#include "HVSCU/Sensors/CurrentSense.hpp"

namespace HVSCU::Sensors {

CurrentSense::CurrentSense(Pin &pin)
    : current_sensor(pin, current_sensor_slope, current_sensor_offset, &value),
      zeroing(current_sensor) {}

void CurrentSense::start() { zeroing.execute(); }

void CurrentSense::read() { current_sensor.read(); }

float *CurrentSense::get_value_pointer() { return &value; }

}  // namespace HVSCU::Sensors