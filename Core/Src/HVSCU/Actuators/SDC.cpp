#include "HVSCU/Actuators/SDC.hpp"

namespace HVSCU::Actuators {

SDC::SDC(Pin& imd_bypass_pin, Pin& sdc_enable_pin, Pin& sdc_state_pin)
    : imd_bypass(imd_bypass_pin),
      sdc_enable(sdc_enable_pin),
      sdc_state(sdc_state_pin, sdc_good) {}

void SDC::enable_imd() { imd_bypass.turn_on(); }

void SDC::disable_imd() { imd_bypass.turn_off(); }

void SDC::enable_sdc() { sdc_enable.turn_on(); }

void SDC::disable_sdc() { sdc_enable.turn_off(); }

void SDC::read_state() {
    sdc_state.read();
    static PinState last_state{sdc_good};
    if (last_state == PinState::ON && sdc_good == PinState::OFF) {
        sdc_good_bool = false;
    }
    last_state = sdc_good;
}

PinState* SDC::get_sdc_state() { return &sdc_good; }
bool* SDC::get_sdc_state_bool() { return &sdc_good_bool; }

}  // namespace HVSCU::Actuators
