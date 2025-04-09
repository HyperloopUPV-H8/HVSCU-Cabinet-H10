#include "HVSCU/Actuators/LEDs.hpp"

namespace HVSCU::Actuators {

LEDs::LEDs(Pin &led_operational_pin, Pin &led_fault_pin, Pin &led_can_pin,
           Pin &led_flash_pin, Pin &led_sleep_pin, Pin &led_full_charge_pin,
           Pin &led_low_charge_pin)
    : led_operational(led_operational_pin),
      led_fault(led_fault_pin),
      led_can(led_can_pin),
      led_flash(led_flash_pin),
      led_sleep(led_sleep_pin),
      led_full_charge(led_full_charge_pin),
      led_low_charge(led_low_charge_pin),
      connecting_blink_action_id(UNDEFINED_ID),
      can_transmit_action_id(UNDEFINED_ID),
      low_charge_blink_action_id(UNDEFINED_ID) {}

void LEDs::signal_connecting() {
    if (connecting_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(connecting_blink_action_id);
        connecting_blink_action_id = UNDEFINED_ID;
    }

    connecting_blink_action_id = Time::register_low_precision_alarm(
        CONNECTING_BLINK_PERIOD_MS, [this]() { led_operational.toggle(); });
}

void LEDs::signal_operational() {
    if (connecting_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(connecting_blink_action_id);
        connecting_blink_action_id = UNDEFINED_ID;
    }

    led_operational.turn_on();
}

void LEDs::signal_fault() {
    if (connecting_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(connecting_blink_action_id);
        connecting_blink_action_id = UNDEFINED_ID;
    }

    led_operational.turn_off();

    led_fault.turn_on();
}

void LEDs::signal_can_transmit() {
    if (can_transmit_action_id != UNDEFINED_ID) {
        return;
    }

    led_can.turn_on();
    can_transmit_action_id =
        Time::set_timeout(CAN_TRANSMIT_ON_TIME_MS, [this]() {
            led_can.turn_off();
            can_transmit_action_id = UNDEFINED_ID;
        });
}

void LEDs::signal_full_charge() {
    if (low_charge_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(low_charge_blink_action_id);
        low_charge_blink_action_id = UNDEFINED_ID;
    }

    led_full_charge.turn_on();
    led_low_charge.turn_off();
}

void LEDs::signal_mid_charge() {
    if (low_charge_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(low_charge_blink_action_id);
        low_charge_blink_action_id = UNDEFINED_ID;
    }

    led_full_charge.turn_off();
    led_low_charge.turn_on();
}

void LEDs::signal_low_charge() {
    if (low_charge_blink_action_id != UNDEFINED_ID) {
        Time::unregister_low_precision_alarm(low_charge_blink_action_id);
        low_charge_blink_action_id = UNDEFINED_ID;
    }

    low_charge_blink_action_id = Time::register_low_precision_alarm(
        LOW_CHARGE_BLINK_PERIOD_MS, [this]() { led_low_charge.toggle(); });
}

};  // namespace HVSCU::Actuators
