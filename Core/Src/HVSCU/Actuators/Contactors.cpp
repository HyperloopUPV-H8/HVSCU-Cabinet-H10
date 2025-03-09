#include "HVSCU/Actuators/Contactors.hpp"

namespace HVSCU::Actuators {

Contactors::NormallyOpenContactor::NormallyOpenContactor(Pin &pin)
    : contactor(pin) {}

void Contactors::NormallyOpenContactor::close() { contactor.turn_on(); }
void Contactors::NormallyOpenContactor::open() { contactor.turn_off(); }
void Contactors::NormallyOpenContactor::to_normal_state() {
    contactor.turn_off();
}

Contactors::NormallyClosedContactor::NormallyClosedContactor(Pin &pin)
    : contactor(pin) {}

void Contactors::NormallyClosedContactor::close() { contactor.turn_off(); }
void Contactors::NormallyClosedContactor::open() { contactor.turn_on(); }
void Contactors::NormallyClosedContactor::to_normal_state() {
    contactor.turn_on();
}

Contactors::Contactors(Pin &ess_discharge_pin, Pin &discharge_pin,
                       Pin &ess_charge_pin, Pin &low_side_pin,
                       Pin &high_side_pin, Pin &precharge_pin,
                       const float &bus_voltage, const float &ess_voltage)
    : ess_discharge(ess_discharge_pin),
      discharge(discharge_pin),
      ess_charge(ess_charge_pin),
      low_side(low_side_pin),
      high_side(high_side_pin),
      precharge(precharge_pin),
      contactors_state(State::Open),
      bus_voltage(bus_voltage),
      ess_voltage(ess_voltage) {
    // States
    contactors_state.add_state(State::Charging);
    contactors_state.add_state(State::Charged);
    contactors_state.add_state(State::Precharge);
    contactors_state.add_state(State::Close);

    // Transitions
    contactors_state.add_transition(State::Open, State::Charging, [&]() {
        return charge_request_received && (ess_voltage < charge_voltage);
    });

    contactors_state.add_transition(State::Open, State::Charged, [&]() {
        return charge_request_received && (ess_voltage >= charge_voltage);
    });

    contactors_state.add_transition(State::Charging, State::Charged, [&]() {
        return ess_voltage >= charge_voltage;
    });

    contactors_state.add_transition(State::Charged, State::Charging, [&]() {
        return charge_request_received && (ess_voltage < charge_voltage);
    });

    contactors_state.add_transition(State::Charged, State::Precharge, [&]() {
        return close_request_received &&
               (bus_voltage < ess_voltage * SAFE_BUS_TO_ESS_RATIO);
    });

    contactors_state.add_transition(State::Charged, State::Close, [&]() {
        return close_request_received &&
               (bus_voltage >= ess_voltage * SAFE_BUS_TO_ESS_RATIO);
    });

    contactors_state.add_transition(State::Precharge, State::Charged,
                                    [&]() { return hold_request_received; });

    contactors_state.add_transition(State::Precharge, State::Close, [&]() {
        return bus_voltage >= ess_voltage * SAFE_BUS_TO_ESS_RATIO;
    });

    contactors_state.add_transition(State::Close, State::Charged,
                                    [&]() { return hold_request_received; });

    // Actions
    contactors_state.add_enter_action(
        [&]() {
            close_discharge_circuit();
            hold_request_received = false;
            charge_request_received = false;
            close_request_received = false;
        },
        State::Open);

    contactors_state.add_enter_action([&]() { close_charging_circuit(); },
                                      State::Charging);

    contactors_state.add_enter_action(
        [&]() {
            close_charged_circuit();
            hold_request_received = false;
            charge_request_received = false;
            close_request_received = false;
        },
        State::Charged);

    contactors_state.add_enter_action([&]() { close_precharge_circuit(); },
                                      State::Precharge);

    contactors_state.add_enter_action(
        [&]() {
            close_high_voltage_circuit();
            hold_request_received = false;
            charge_request_received = false;
            close_request_received = false;
        },
        State::Close);

    state_check_alarm_id = Time::register_low_precision_alarm(
        CHECK_PERIOD_MS, [this]() { contactors_state.check_transitions(); });

    contactors_state.force_change_state(State::Open);
}

Contactors::~Contactors() {
    Time::unregister_low_precision_alarm(state_check_alarm_id);
    close_discharge_circuit();
}

void Contactors::close_discharge_circuit() {
    ess_discharge.close();
    discharge.close();

    ess_charge.open();
    low_side.open();
    high_side.open();
    precharge.open();
}

void Contactors::close_charging_circuit() {
    ess_discharge.open();
    discharge.close();

    ess_charge.close();
    low_side.open();
    high_side.open();
    precharge.open();
}

void Contactors::close_charged_circuit() {
    ess_discharge.open();
    discharge.close();

    ess_charge.open();
    low_side.open();
    high_side.open();
    precharge.open();
}

void Contactors::close_precharge_circuit() {
    ess_discharge.open();
    discharge.open();

    ess_charge.open();
    low_side.close();
    high_side.open();
    precharge.close();
}

void Contactors::close_high_voltage_circuit() {
    ess_discharge.open();
    discharge.open();

    ess_charge.open();
    low_side.close();
    high_side.close();
    precharge.open();
}

void Contactors::charge(float target_voltage) {
    if (get_state() != State::Open && get_state() != State::Charged &&
        get_state() != State::Charging)
        return;

    charge_voltage = target_voltage;
    charge_request_received = true;
}

void Contactors::close() {
    if (get_state() != State::Charged) return;

    close_request_received = true;
}

void Contactors::hold_charge() {
    if (get_state() != State::Precharge && get_state() != State::Close) return;

    hold_request_received = true;
}

// Having the open transition behind a boolean flag could lead to catastrophic
// logic errors, where the  board cannot open the contactors in case of an
// emergency
void Contactors::open() { contactors_state.force_change_state(State::Open); }

const StateMachine::state_id &Contactors::get_state() const {
    return contactors_state.current_state;
}

};  // namespace HVSCU::Actuators
