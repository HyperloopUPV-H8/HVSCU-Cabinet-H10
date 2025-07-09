#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : imd(Pinout::imd_power_pin, Pinout::imd_measurement_high_side_pin),
      supercaps_voltage(Pinout::supercaps_voltage_measurement_pin),
      current_sense(Pinout::output_current_measurement_pin),
      contactors(Pinout::contactor_ess_discharge_pin,
                 Pinout::contactor_inverter_discharge_pin,
                 Pinout::contactor_ess_charge_pin, Pinout::contactor_low_pin,
                 Pinout::contactor_high_pin, Pinout::contactor_precharge_pin,
                 bus_voltage_value, ess_voltage),

      leds(Pinout::led_operational_pin, Pinout::led_fault_pin,
           Pinout::led_can_pin, Pinout::led_flash_pin, Pinout::led_sleep_pin,
           Pinout::led_full_charge_pin, Pinout::led_low_charge_pin),
      sdc(Pinout::imd_bypass_pin, Pinout::sdc_control_pin,
          Pinout::sdc_good_pin),
      can(),
      stlib(HVSCU::Communication::Ethernet::local_ip.string_address,
            "255.255.0.0", "192.168.2.1"),
      ethernet(
          &can.module_can.system.total_voltage_volts,
          can.module_can.system.all_cells_voltage[0],
          can.module_can.system.all_module_voltage[0],
          can.module_can.system.all_max_cell_voltage[0],
          can.module_can.system.all_min_cell_voltage[0],
          can.module_can.system.all_avg_cell_voltage[0],
          can.module_can.system.all_max_temperature[0],
          can.module_can.system.all_min_temperature[0], sdc.get_sdc_state(),
          &bus_voltage_value, supercaps_voltage.get_voltage_pointer(),
          contactors.get_state_pointer(), current_sense.get_value_pointer()) {
    populate_state_machine();
    leds.signal_connecting();
    initialize_protections();

    can.start();

    Time::register_low_precision_alarm(100,
                                       [&]() { send_ethernet_10hz = true; });

    Time::register_low_precision_alarm(1, [&]() { send_ethernet_1khz = true; });

    Time::register_low_precision_alarm(100,
                                       [&]() { read_sensors_10hz = true; });

    Time::register_low_precision_alarm(1, [&]() { read_sensors_1khz = true; });
}

void Board::update() {
    general_state_machine.check_transitions();
    switch (general_state_machine.current_state) {
        case States::CONNECTING:
            update_connecting();
            break;
        case States::OPERATIONAL:
            update_operational();
            break;
        case States::FAULT:
            update_fault();
            break;
    }

    if (send_ethernet_1khz) {
        ethernet.send_current_sense();

        send_ethernet_1khz = false;
    }

    if (send_ethernet_10hz) {
        ethernet.send_supercaps_data();
        ethernet.send_sdc_data();
        ethernet.send_contactors_data();

        send_ethernet_10hz = false;
    }

    if (read_sensors_10hz) {
        sdc.read_state();
        supercaps_voltage.read();
        ess_voltage = can.module_can.system.total_voltage_volts;

        read_sensors_10hz = false;
    }

    if (read_sensors_1khz) {
        current_sense.read();

        read_sensors_1khz = false;
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
        ethernet.has_received_hold_supercaps = false;
        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    } else if (ethernet.has_received_hold_supercaps) {
        contactors.hold_charge();

        ethernet.has_received_hold_supercaps = false;
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
}

void Board::update_fault() {}

void Board::populate_state_machine() {
    // GENERAL STATE MACHINE
    general_state_machine.add_state(States::OPERATIONAL);
    general_state_machine.add_state(States::FAULT);

    //     TRANSITIONS
    general_state_machine.add_transition(
        States::CONNECTING, States::OPERATIONAL,
        [this]() { return ethernet.is_connected(); });

    //     ENTER ACTIONS
    general_state_machine.add_enter_action(
        [this]() { leds.signal_connecting(); }, States::CONNECTING);

    general_state_machine.add_enter_action(
        [this]() { leds.signal_operational(); }, States::OPERATIONAL);

    general_state_machine.add_enter_action(
        [this]() {
            contactors.open();
            leds.signal_fault();
        },
        States::FAULT);

    general_state_machine.add_enter_action([this]() {}, States::FAULT);
}

void Board::initialize_protections() {}

};  // namespace HVSCU
