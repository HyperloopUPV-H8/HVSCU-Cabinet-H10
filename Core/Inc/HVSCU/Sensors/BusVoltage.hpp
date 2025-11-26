#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Sensors {

class BusVoltage {
    constexpr static float slope{121.93889617919922f};
    constexpr static float offset{-8.31775951385498f};

    LinearSensor<float> voltage_sensor;

    float voltage;

   public:
    BusVoltage(Pin &sensor_pin);

    void read();

    float *get_voltage_pointer();
    float &get_voltage();
};

};  // namespace HVSCU::Sensors