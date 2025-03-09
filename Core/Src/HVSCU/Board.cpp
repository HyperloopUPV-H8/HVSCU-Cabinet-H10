#include "HVSCU/Board.hpp"

namespace HVSCU {

Board::Board()
    : imd(Pinout::imd_power_pin, Pinout::imd_bypass_pin,
          Pinout::imd_measurement_high_side_pin),
      contactors(Pinout::contactor_ess_discharge_pin,
                 Pinout::contactor_inverter_discharge_pin,
                 Pinout::contactor_ess_charge_pin, Pinout::contactor_low_pin,
                 Pinout::contactor_high_pin, Pinout::contactor_precharge_pin,
                 bus_voltage, can.module_can.system.total_voltage_volts),
      leds(Pinout::led_operational_pin, Pinout::led_fault_pin,
           Pinout::led_can_pin, Pinout::led_flash_pin, Pinout::led_sleep_pin,
           Pinout::led_full_charge_pin, Pinout::led_low_charge_pin),
      can() {
    STLIB::start();
    initialize_state_machine();
}

void Board::initialize_state_machine() {}

void Board::update() {
    can.update();
    imd.update();
    STLIB::update();
}

};  // namespace HVSCU
