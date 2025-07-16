#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Sensors {

class IMD {
   public:
    enum State : uint8_t {
        SHORT_CIRCUIT = 0,
        OK,
        UNDERVOLTAGE,
        FAST_START,
        EQUIPMENT_FAULT,
        GROUNDING_FAULT,
        UNKNOWN
    };

    // from the datasheet this is the max value it can detect (in ohms)
    constexpr static float infinite_resistance{50000000};

   private:
    DigitalOutput power;

    PinState ok_state{PinState::OFF};
    DigitalSensor ok;

    PWMSensor<float> output;

    float output_frequency{0.0};
    float output_duty_cycle{0.0};

    State state{State::UNKNOWN};
    float isolation_resistance{0.0};

   public:
    IMD(Pin &power_pin, Pin &output_pin, Pin &ok_pin);

    bool ever_got_ok{false};

    void turn_on();
    void turn_off();

    State *get_state();
    float *get_isolation_resistance();
    PinState *get_ok_state();

    void update();
};

};  // namespace HVSCU::Sensors