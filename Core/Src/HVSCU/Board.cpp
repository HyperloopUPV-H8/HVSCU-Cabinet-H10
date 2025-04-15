#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : imd(Pinout::imd_power_pin, Pinout::imd_measurement_high_side_pin),
      bus_voltage(Pinout::bus_voltage_measurement_pin),
      current_sense(Pinout::output_current_measurement_pin),
      contactors(Pinout::contactor_ess_discharge_pin,
                 Pinout::contactor_inverter_discharge_pin,
                 Pinout::contactor_ess_charge_pin, Pinout::contactor_low_pin,
                 Pinout::contactor_high_pin, Pinout::contactor_precharge_pin,
                 bus_voltage.get_voltage(), ess_voltage),
      leds(Pinout::led_operational_pin, Pinout::led_fault_pin,
           Pinout::led_can_pin, Pinout::led_flash_pin, Pinout::led_sleep_pin,
           Pinout::led_full_charge_pin, Pinout::led_low_charge_pin),
      sdc(Pinout::imd_bypass_pin, Pinout::sdc_control_pin,
          Pinout::sdc_good_pin),
      can(),
      stlib(HVSCU::Communication::Ethernet::local_ip.string_address,
            "255.255.0.0", "192.168.2.1"),
      ethernet(&can.module_can.system.total_voltage_volts,
               can.module_can.system.all_cells_voltage[0],
               can.module_can.system.all_module_voltage[0],
               can.module_can.system.all_max_cell_voltage[0],
               can.module_can.system.all_min_cell_voltage[0],
               can.module_can.system.all_avg_cell_voltage[0],
               can.module_can.system.all_max_temperature[0],
               can.module_can.system.all_min_temperature[0],
               sdc.get_sdc_state(), bus_voltage.get_voltage_pointer(),
               contactors.get_state_pointer(),
               current_sense.get_value_pointer(), &can.master_general_state,
               &can.master_nested_state, &can.slave_general_state,
               &can.slave_nested_state, &can.duty_cycle_u, &can.duty_cycle_v,
               &can.duty_cycle_w) {
    initialize_state_machine();

    can.start();

    Time::register_low_precision_alarm(100, [&]() {
        ethernet.send_supercaps_data();
        ethernet.send_sdc_data();
        ethernet.send_contactors_data();
        ethernet.send_bcu_data();
        ethernet.send_current_sense();
    });

    Time::register_low_precision_alarm(100,
                                       [&]() { ethernet.send_bcu_data(); });

    Time::register_low_precision_alarm(100, [&]() {
        sdc.read_state();
        bus_voltage.read();
        current_sense.read();
        ess_voltage = can.module_can.system.total_voltage_volts;
    });
}

void Board::initialize_state_machine() {
    general_state_machine = StateMachine{States::Connecting};
    general_state_machine.add_state(States::Operational);
    general_state_machine.add_state(States::Fault);

    // Transitions
    ProtectionManager::link_state_machine(general_state_machine, States::Fault);

    general_state_machine.add_transition(
        States::Connecting, States::Operational,
        [this]() { return ethernet.is_connected(); });

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
    general_state_machine.check_transitions();
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
    stlib.update();
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

        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    }

    if (ethernet.has_received_reset_supercaps) {
        can.restart();

        ethernet.has_received_reset_supercaps = false;
    }

    if (ethernet.has_received_imd_bypass) {
        if (ethernet.imd_bypass_state ==
            Communication::Ethernet::ImdBypassState::EnableIMD) {
            sdc.enable_imd();
        } else {
            sdc.disable_imd();
        }

        ethernet.has_received_imd_bypass = false;
    }

    if (ethernet.has_received_sdc_enable) {
        if (ethernet.sdc_enable_state ==
            Communication::Ethernet::SdcEnableState::EnableSDC) {
            sdc.enable_sdc();
        } else {
            sdc.disable_sdc();
        }

        ethernet.has_received_sdc_enable = false;
    }

    if (ethernet.has_received_BCU_test_pwm) {
        can.transmit_start_test_pwm(ethernet.requested_duty_cycle_u,
                                    ethernet.requested_duty_cycle_v,
                                    ethernet.requested_duty_cycle_w);

        ethernet.has_received_BCU_test_pwm = false;
    }

    if (ethernet.has_received_BCU_configure_commutation_parameters) {
        can.transmit_configure_commutation_parameters(
            ethernet.requested_commutation_frequency_hz,
            ethernet.requested_dead_time_ns);

        ethernet.has_received_BCU_configure_commutation_parameters = false;
    }

    if (ethernet.has_received_BCU_stop) {
        can.transmit_stop();

        ethernet.has_received_BCU_stop = false;
    }

    if (ethernet.has_received_BCU_space_vector) {
        can.transmit_start_space_vector(
            ethernet.requested_modulation_index,
            ethernet.requested_modulation_frequency_hz);

        ethernet.has_received_BCU_space_vector = false;
    }
}

void Board::update_fault() {}

};  // namespace HVSCU
