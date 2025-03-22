#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : can(),
      imd(Pinout::imd_power_pin, Pinout::imd_bypass_pin,
          Pinout::imd_measurement_high_side_pin),
      contactors(Pinout::contactor_ess_discharge_pin,
                 Pinout::contactor_inverter_discharge_pin,
                 Pinout::contactor_ess_charge_pin, Pinout::contactor_low_pin,
                 Pinout::contactor_high_pin, Pinout::contactor_precharge_pin,
                 bus_voltage, can.module_can.system.total_voltage_volts),
      ethernet(&can.module_can.system.total_voltage_volts,
               can.module_can.system.all_cells_voltage[0],
               can.module_can.system.all_module_voltage[0],
               can.module_can.system.all_max_cell_voltage[0],
               can.module_can.system.all_min_cell_voltage[0],
               can.module_can.system.all_avg_cell_voltage[0],
               can.module_can.system.all_max_temperature[0],
               can.module_can.system.all_min_temperature[0]),
      leds(Pinout::led_operational_pin, Pinout::led_fault_pin,
           Pinout::led_can_pin, Pinout::led_flash_pin, Pinout::led_sleep_pin,
           Pinout::led_full_charge_pin, Pinout::led_low_charge_pin) {
    STLIB::start();
    initialize_state_machine();
}

void Board::initialize_state_machine() {
    general_state_machine = StateMachine{States::Connecting};
    general_state_machine.add_state(States::Operational);
    general_state_machine.add_state(States::Fault);

    // Transitions
    ProtectionManager::link_state_machine(general_state_machine, States::Fault);

    general_state_machine.add_transition(
        States::Connecting, States::Operational,
        [this]() { return imd.get_state() == Sensors::IMD::State::Ok; });

    // Enter Actions
    // State LEDs
    general_state_machine.add_enter_action(
        [this]() { leds.signal_connecting(); }, States::Connecting);

    general_state_machine.add_enter_action(
        [this]() { leds.signal_operational(); }, States::Operational);

    general_state_machine.add_enter_action([this]() { leds.signal_fault(); },
                                           States::Fault);

    // Contactors
    general_state_machine.add_enter_action([this]() { contactors.open(); },
                                           States::Fault);

    // Ensures the enter action of the initial state runs
    general_state_machine.force_change_state(States::Connecting);
}

void Board::update() {
    switch (general_state_machine.current_state) {
        case States::Connecting:
            update_connecting();
            break;
        case States::Operational:
            update_operational();
            break;
        case States::Fault:
            update_fault();
            break;
    }

    can.update();
    imd.update();
    STLIB::update();
}

void Board::update_connecting() {}

void Board::update_operational() {
    if (ethernet.has_received_open_contactors) {
        contactors.open();

        ethernet.has_received_open_contactors = false;
        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    } else if (ethernet.has_received_charge_supercaps) {
        contactors.charge(ethernet.charge_voltage);

        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    } else if (ethernet.has_received_close_contactors) {
        contactors.close();

        ethernet.has_received_close_contactors = false;
    }
}

void Board::update_fault() {}

};  // namespace HVSCU
