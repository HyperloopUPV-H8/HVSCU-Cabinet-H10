#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Actuators {

class SDC {
    DigitalOutput imd_bypass;
    DigitalOutput sdc_enable;

    PinState sdc_good{PinState::OFF};
    DigitalSensor sdc_state;

    bool sdc_good_bool{true};

   public:
    SDC(Pin& imd_bypass_pin, Pin& sdc_enable_pin, Pin& sdc_state_pin);

    void enable_imd();
    void disable_imd();

    void enable_sdc();
    void disable_sdc();

    void read_state();
    PinState* get_sdc_state();
    bool* get_sdc_state_bool();
};

};  // namespace HVSCU::Actuators