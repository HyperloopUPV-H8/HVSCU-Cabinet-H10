#include "HVSCU/Pinout.hpp"

// Pin tags as they are in the board schematic
#define FW_CONTACTOR_LOW PG14
#define FW_CONTACTOR_HIGH PG12
#define FW_PRECHARGE PD4
#define TXD PD1
#define RXD PD0
#define SDC_OBCCU PA11
#define SLNT PA8
#define LED_OPERATIONAL PG8
#define LED_FAULT PG7
#define LED_CAN PG6
#define LED_FLASH PG5
#define LED_SLEEP PG4
#define LED_FULL_CHARGE PG3
#define LED_LOW_CHARGE PG2
#define SDC_GOOD PB12
#define HV_3V3 PF13
#define AI_BATTERY_CURRENT_FW PA0
#define HV_PSU PF10
#define HV_CHARGER PF9
#define DISCHARGE_SC PF8
#define IMD_MHS_3V3 PF7
#define IMD_MLS_3V3 PF6
#define FW_IMD_BYPASS PF5
#define FW_DISCHARGE PF4
#define IMD_POW PE2

namespace HVSCU::Pinout {

Pin& output_current_measurement_pin{AI_BATTERY_CURRENT_FW};
Pin& bus_voltage_measurement_pin{HV_3V3};

Pin& high_voltage_power_supply_enable_pin{HV_PSU};

Pin& sdc_control_pin{SDC_OBCCU};
Pin& sdc_good_pin{SDC_GOOD};

Pin& imd_power_pin{IMD_POW};
Pin& imd_bypass_pin{FW_IMD_BYPASS};
Pin& imd_measurement_high_side_pin{IMD_MHS_3V3};
Pin& imd_measurement_low_side_pin{IMD_MLS_3V3};

Pin& contactor_high_pin{FW_CONTACTOR_HIGH};
Pin& contactor_low_pin{FW_CONTACTOR_LOW};
Pin& contactor_precharge_pin{FW_PRECHARGE};
Pin& contactor_ess_discharge_pin{DISCHARGE_SC};
Pin& contactor_ess_charge_pin{HV_CHARGER};
Pin& contactor_inverter_discharge_pin{FW_DISCHARGE};

Pin& can_tx_pin{TXD};
Pin& can_rx_pin{RXD};
Pin& can_silent_mode_pin{SLNT};

Pin& led_operational_pin{LED_OPERATIONAL};
Pin& led_fault_pin{LED_FAULT};
Pin& led_can_pin{LED_CAN};
Pin& led_flash_pin{LED_FLASH};
Pin& led_sleep_pin{LED_SLEEP};
Pin& led_full_charge_pin{LED_FULL_CHARGE};
Pin& led_low_charge_pin{LED_LOW_CHARGE};

};  // namespace HVSCU::Pinout
