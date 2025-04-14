#pragma once

#include "ST-LIB.hpp"
#include "ST-LIB_HIGH/Control/Blocks/Zeroing.hpp"

namespace HVSCU::Sensors {

class CurrentSense {
    constexpr static float current_sensor_offset{0.0f};
    constexpr static float current_sensor_slope{1.0f};

    float value{0.0};

    LinearSensor<float> current_sensor;
    Zeroing<float, 1000> zeroing;

   public:
    CurrentSense(Pin &pin);

    void start();

    void read();

    float *get_value_pointer();
};

}  // namespace HVSCU::Sensors