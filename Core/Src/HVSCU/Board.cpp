#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : imd(Pinout::imd_power_pin, Pinout::imd_measurement_high_side_pin),
      bus_voltage(Pinout::bus_voltage_measurement_pin),
      contactors(
          Pinout::contactor_ess_discharge_pin,
          Pinout::contactor_inverter_discharge_pin,
          Pinout::contactor_ess_charge_pin, Pinout::contactor_low_pin,
          Pinout::contactor_high_pin, Pinout::contactor_precharge_pin,
          bus_voltage.get_voltage(),
          bus_voltage
              .get_voltage()),  // can.module_can.system.total_voltage_volts),
      leds(Pinout::led_operational_pin, Pinout::led_fault_pin,
           Pinout::led_can_pin, Pinout::led_flash_pin, Pinout::led_sleep_pin,
           Pinout::led_full_charge_pin, Pinout::led_low_charge_pin),
      sdc(Pinout::imd_bypass_pin, Pinout::sdc_control_pin,
          Pinout::sdc_good_pin),
      stlib(HVSCU::Communication::Ethernet::local_ip.string_address,
            "255.255.0.0", "192.168.2.1"),
      can(),
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
                BCU_data::charge_voltage,
               BCU_data::duty_u, BCU_data::duty_v, BCU_data::duty_w,
               BCU_data::angular_velocity, BCU_data::ref_q, BCU_data::ref_d,
               BCU_data::ref_speed, BCU_data::ref_switch_freq,
               BCU_data::ref_dead_time, BCU_data::fixed_dc_link_vtg) {
    initialize_state_machine();

    can.start();
    Time::register_low_precision_alarm(100, [&]() {
        ethernet.send_supercaps_data();
        ethernet.send_sdc_data();
        ethernet.send_contactors_data();
    });

    Time::register_low_precision_alarm(100, [&]() {
        sdc.read_state();
        bus_voltage.read();
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
        // Handle BCU test PWM logic here
        can
        ethernet.has_received_BCU_test_pwm = false;
    }

    if (ethernet.has_received_BCU_emulate_movement) {
        // Handle BCU emulate movement logic here
        ethernet.has_received_BCU_emulate_movement = false;
    }

    if (ethernet.has_received_BCU_current_control) {
        // Handle BCU current control logic here
        ethernet.has_received_BCU_current_control = false;
    }

    if (ethernet.has_received_BCU_velocity_control) {
        // Handle BCU velocity control logic here
        ethernet.has_received_BCU_velocity_control = false;
    }

    if (ethernet.has_received_BCU_set_pwm_params) {
        // Handle BCU set PWM parameters logic here
        ethernet.has_received_BCU_set_pwm_params = false;
    }

    if (ethernet.has_received_BCU_stop) {
        // Handle BCU stop logic here
        ethernet.has_received_BCU_stop = false;
    }

    if (ethernet.has_received_BCU_set_fixed_dc_link_vtg) {
        // Handle BCU set fixed DC link voltage logic here
        ethernet.has_received_BCU_set_fixed_dc_link_vtg = false;
    }

    if (ethernet.has_received_BCU_unset_fixed_dc_link_vtg) {
        // Handle BCU unset fixed DC link voltage logic here
        ethernet.has_received_BCU_unset_fixed_dc_link_vtg = false;
    }
}

void Board::update_fault() {}

};  // namespace HVSCU
