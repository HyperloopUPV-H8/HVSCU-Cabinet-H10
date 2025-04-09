#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Actuators {

class LEDs {
    static constexpr uint32_t CONNECTING_BLINK_PERIOD_MS{500};
    static constexpr uint32_t CAN_TRANSMIT_ON_TIME_MS{100};
    static constexpr uint32_t LOW_CHARGE_BLINK_PERIOD_MS{1000};
    static constexpr uint8_t UNDEFINED_ID{std::numeric_limits<uint8_t>::max()};

    DigitalOutput led_operational;
    DigitalOutput led_fault;
    DigitalOutput led_can;
    DigitalOutput led_flash;
    DigitalOutput led_sleep;
    DigitalOutput led_full_charge;
    DigitalOutput led_low_charge;

    uint8_t connecting_blink_action_id{UNDEFINED_ID};
    uint8_t can_transmit_action_id{UNDEFINED_ID};
    uint8_t low_charge_blink_action_id{UNDEFINED_ID};

   public:
    LEDs(Pin &led_operational_pin, Pin &led_fault_pin, Pin &led_can_pin,
         Pin &led_flash_pin, Pin &led_sleep_pin, Pin &led_full_charge_pin,
         Pin &led_low_charge_pin);

    void signal_connecting();

    void signal_operational();

    void signal_fault();

    void signal_can_transmit();

    void signal_full_charge();

    void signal_mid_charge();

    void signal_low_charge();
};

};  // namespace HVSCU::Actuators