#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : imd(Pinout::imd_power_pin, Pinout::imd_measurement_low_side_pin,
          Pinout::imd_ok_pin),
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
      stlib("00:80:e1:00:02:16",
            HVSCU::Communication::Ethernet::local_ip.string_address,
            "255.255.0.0", "192.168.2.1"),
      ethernet(
          &can.module_can.system.total_voltage_volts, &state_of_charge,
          can.module_can.system.all_cells_voltage[0],
          can.module_can.system.all_module_voltage[0],
          can.module_can.system.all_max_cell_voltage[0],
          can.module_can.system.all_min_cell_voltage[0],
          can.module_can.system.all_avg_cell_voltage[0],
          can.module_can.system.all_max_temperature[0],
          can.module_can.system.all_min_temperature[0], sdc.get_sdc_state(),
          &bus_voltage_value, supercaps_voltage.get_voltage_pointer(),
          contactors.get_state_pointer(), current_sense.get_value_pointer(),
          imd.get_state(), imd.get_isolation_resistance(), imd.get_ok_state()) {
    sdc.enable_sdc();
    imd.turn_on();
    HAL_Delay(3000);

    populate_state_machine();
    leds.signal_connecting();
    initialize_protections();

    can.start();

    Time::register_low_precision_alarm(100, [&]() {
        send_ethernet_10hz = true;
        read_sensors_10hz = true;
    });

    Time::register_low_precision_alarm(1, [&]() {
        send_ethernet_1khz = true;
        read_sensors_1khz = true;
    });

    // Time::register_low_precision_alarm(
    //     1, [&]() { protection_manager.update_low_frequency(); });
}

void Board::update() {
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
        state_of_charge = ess_voltage * 100.0f / MAX_ESS_VOLTAGE;
        for (uint8_t i = 0; i < 3; i++) {
            // if (*can.module_can.system.all_module_voltage[0][i] >= 148.8f) {
            //     ProtectionManager::fault_and_propagate();
            //     break;
            // }

            // if (*can.module_can.system.all_max_temperature[0][i] >= 60.0) {
            //     ProtectionManager::fault_and_propagate();
            //     break;
            // }

            // if (*can.module_can.system.all_min_temperature[0][i] <= 5.5) {
            //     ProtectionManager::fault_and_propagate();
            //     break;
            // }

            bool protection_triggered = false;
            // for (uint8_t j = 0; j < 48; j++) {
            //     if (*can.module_can.system.all_cells_voltage[0][i][j] >= 3.1f) {
            //         ProtectionManager::fault_and_propagate();
            //         protection_triggered = true;
            //         break;
            //     }
            // }
            if (protection_triggered) break;
        }

        read_sensors_10hz = false;
    }

    if (read_sensors_1khz) {
        current_sense.read();

        read_sensors_1khz = false;
    }

    can.update();
    imd.update();
    imd_fault = imd.ever_got_ok && *imd.get_ok_state() == PinState::OFF;
    protection_manager.update_high_frequency();
    general_state_machine.check_transitions();
    [[maybe_unused]] Actuators::Contactors::State pruebas = *(contactors.get_state_pointer());
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
        if (*sdc.get_sdc_state_bool()) contactors.hold_charge();

        ethernet.has_received_hold_supercaps = false;
        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    } else if (ethernet.has_received_charge_supercaps) {
        if (*sdc.get_sdc_state_bool())
            contactors.charge(ethernet.charge_voltage);

        ethernet.has_received_charge_supercaps = false;
        ethernet.has_received_close_contactors = false;
    } else if (ethernet.has_received_close_contactors) {
        if (*sdc.get_sdc_state_bool()) contactors.close();

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
            contactors.fault();
            sdc.disable_sdc();
            leds.signal_fault();
        },
        States::FAULT);

    general_state_machine.add_enter_action([this]() {}, States::FAULT);
}

void Board::initialize_protections() {
    add_protection(&imd_fault, Boundary<bool, EQUALS>(true));
    add_protection(&can.module_can.keepalive_expired,
                   Boundary<bool, EQUALS>(true));
    add_protection(sdc.get_sdc_state_bool(), Boundary<bool, EQUALS>(false));

    add_protection(current_sense.get_value_pointer(),
                   Boundary<float, OUT_OF_RANGE>(-10.0f, 70.0f, -20.0, 100.0f));

    add_protection(supercaps_voltage.get_voltage_pointer(),
                   Boundary<float, ABOVE>(400.0f, 446.4f));

    add_protection(&can.module_can.system.total_voltage_volts,
                   Boundary<float, ABOVE>(400.0f, 446.4f));
}

};  // namespace HVSCU
