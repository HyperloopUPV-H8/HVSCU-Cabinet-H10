#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Actuators {

class Contactors {
   public:
    enum State : uint8_t {
        Open = 0,
        Charging = 1,
        Charged = 2,
        Precharge = 3,
        Close = 4
    };

   private:
    constexpr static float SAFE_BUS_TO_ESS_RATIO{0.9f};
    constexpr static uint32_t CHECK_PERIOD_MS{100};

    class NormallyOpenContactor {
        DigitalOutput contactor;

       public:
        NormallyOpenContactor(Pin &pin);

        void close();
        void open();
        void to_normal_state();
    };

    struct NormallyClosedContactor {
        DigitalOutput contactor;

       public:
        NormallyClosedContactor(Pin &pin);

        void close();
        void open();
        void to_normal_state();
    };

    NormallyClosedContactor ess_discharge;
    NormallyClosedContactor discharge;

    NormallyOpenContactor ess_charge;
    NormallyOpenContactor low_side;
    NormallyOpenContactor high_side;
    NormallyOpenContactor precharge;

    uint8_t state_check_alarm_id;
    StateMachine contactors_state;

    float charge_voltage{0.0f};
    bool charge_request_received{false};
    bool close_request_received{false};
    bool hold_request_received{false};

    const float &bus_voltage;
    const float &ess_voltage;

    void close_discharge_circuit();
    void close_charging_circuit();
    void close_charged_circuit();
    void close_precharge_circuit();
    void close_high_voltage_circuit();

   public:
    Contactors(Pin &ess_discharge_pin, Pin &discharge_pin, Pin &ess_charge_pin,
               Pin &low_side_pin, Pin &high_side_pin, Pin &precharge_pin,
               const float &bus_voltage, const float &ess_voltage);

    void charge(float target_voltage);
    void close();

    void open();
    void hold_charge();

    const StateMachine::state_id &get_state() const;
    State *get_state_pointer();

    ~Contactors();
};

};  // namespace HVSCU::Actuators
