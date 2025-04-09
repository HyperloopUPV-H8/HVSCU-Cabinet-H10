#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Pinout {

extern Pin& output_current_measurement_pin;
extern Pin& bus_voltage_measurement_pin;

extern Pin& high_voltage_power_supply_enable_pin;

extern Pin& sdc_control_pin;
extern Pin& sdc_good_pin;

extern Pin& imd_power_pin;
extern Pin& imd_bypass_pin;
extern Pin& imd_measurement_high_side_pin;
extern Pin& imd_measurement_low_side_pin;

extern Pin& contactor_high_pin;
extern Pin& contactor_low_pin;
extern Pin& contactor_precharge_pin;
extern Pin& contactor_ess_discharge_pin;
extern Pin& contactor_ess_charge_pin;
extern Pin& contactor_inverter_discharge_pin;

extern Pin& can_tx_pin;
extern Pin& can_rx_pin;
extern Pin& can_silent_mode_pin;

extern Pin& led_operational_pin;
extern Pin& led_fault_pin;
extern Pin& led_can_pin;
extern Pin& led_flash_pin;
extern Pin& led_sleep_pin;
extern Pin& led_full_charge_pin;
extern Pin& led_low_charge_pin;

};  // namespace HVSCU::Pinout