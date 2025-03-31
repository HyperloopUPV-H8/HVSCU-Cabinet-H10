#include "HVSCU/Sensors/BusVoltage.hpp"

namespace HVSCU::Sensors {

BusVoltage::BusVoltage(Pin &sensor_pin)
    : voltage_sensor(sensor_pin, slope, offset, voltage) {}

void BusVoltage::read() { voltage_sensor.read(); }

float *BusVoltage::get_voltage_pointer() { return &voltage; }
float &BusVoltage::get_voltage() { return voltage; }

}  // namespace HVSCU::Sensors
