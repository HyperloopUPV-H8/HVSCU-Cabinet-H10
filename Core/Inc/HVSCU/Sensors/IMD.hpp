#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Sensors {

class IMD {
   public:
    enum class State : uint8_t {
        ShortCircuit = 0,
        Ok = 1,
        Undervoltage = 2,
        FastStart = 3,
        EquipmentFault = 4,
        GroundingFault = 5,
        Unknown = 6
    };

    // from the datasheet this is the max value it can detect (in ohms)
    constexpr static float infinite_resistance{50000000};

   private:
    DigitalOutput power;

    PWMSensor<float> output;

    float output_frequency{0.0};
    float output_duty_cycle{0.0};

    State state{State::Unknown};
    float isolation_resistance{0.0};

   public:
    IMD(Pin &power_pin, Pin &output_pin);

    void turn_on();
    void turn_off();

    const State &get_state() const;
    const float &get_isolation_resistance() const;

    void update();
};

};  // namespace HVSCU::Sensors