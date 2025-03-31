#include "HVSCU/Sensors/IMD.hpp"

// should be exact frequency values, but measurements are never 100% accurate
#define SHORTCIRCUIT_STATE_FREQ_THRESHOLD 5
#define OK_STATE_FREQ_THRESHOLD 15
#define UNDERVOLTAGE_STATE_FREQ_THRESHOLD 25
#define FAST_START_STATE_FREQ_THRESHOLD 35
#define EQUIPMENT_FAULT_STATE_FREQ_THRESHOLD 45
#define GROUNDING_FAULT_STATE_FREQ_THRESHOLD 55

namespace HVSCU::Sensors {

IMD::IMD(Pin &power_pin, Pin &output_pin)
    : power(power_pin),
      output(output_pin, output_frequency, output_duty_cycle) {
    turn_off();
}

void IMD::turn_on() { power.turn_on(); }
void IMD::turn_off() {
    power.turn_off();
    // cannot know the state when IMD is off :/
    state = IMD::State::Unknown;
    isolation_resistance = infinite_resistance;
}

const IMD::State &IMD::get_state() const { return state; }
const float &IMD::get_isolation_resistance() const {
    return isolation_resistance;
}

#define Kohms(ohms) (ohms * 1000000.0f)

inline float duty_cycle_to_resistance(const float &duty_cycle) {
    if (duty_cycle <= 5.0f)
        return IMD::infinite_resistance;
    else if (duty_cycle >= 95.0f)
        return 0.0f;
    else
        return (90.0f * Kohms(12.0f) / (duty_cycle - 5.0f)) - Kohms(12.0f);
}

void IMD::update() {
    if (output_frequency <= SHORTCIRCUIT_STATE_FREQ_THRESHOLD) {
        state = IMD::State::ShortCircuit;
        isolation_resistance = 0.0f;
    } else if (output_frequency <= OK_STATE_FREQ_THRESHOLD) {
        state = IMD::State::Ok;
        isolation_resistance = duty_cycle_to_resistance(output_duty_cycle);
    } else if (output_frequency <= UNDERVOLTAGE_STATE_FREQ_THRESHOLD) {
        state = IMD::State::Undervoltage;
        isolation_resistance = duty_cycle_to_resistance(output_duty_cycle);
    } else if (output_frequency <= FAST_START_STATE_FREQ_THRESHOLD) {
        state = IMD::State::FastStart;

        // Really between 5%..10% is ok and between 90%..95% is bad. Not at 50%
        // because on other states, the range 47.5%..52.5% is used to signal an
        // error, zero isolation resistance should trigger a fault
        if (output_duty_cycle <= 35.0f)
            isolation_resistance = infinite_resistance;
        else
            isolation_resistance = 0.0f;
    } else if (output_frequency <= EQUIPMENT_FAULT_STATE_FREQ_THRESHOLD) {
        state = IMD::State::EquipmentFault;
        isolation_resistance = 0.0f;
    } else if (output_frequency <= GROUNDING_FAULT_STATE_FREQ_THRESHOLD) {
        state = IMD::State::GroundingFault;
        isolation_resistance = 0.0f;
    } else {
        state = IMD::State::Unknown;
        isolation_resistance = 0.0f;
    }
}

};  // namespace HVSCU::Sensors
