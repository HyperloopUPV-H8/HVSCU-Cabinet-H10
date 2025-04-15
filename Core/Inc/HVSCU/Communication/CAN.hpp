#pragma once

#include "CMS-LIB.hpp"
#include "ST-LIB.hpp"
namespace HVSCU::Communication {

class CAN {
    inline constexpr static uint8_t DLC_to_length(
        const ::FDCAN::DLC dlc) noexcept;

    inline constexpr static ::FDCAN::DLC length_to_DLC(
        const uint8_t length) noexcept;

    constexpr static uint16_t state_id{0x721};
    constexpr static uint16_t start_test_pwm_id{0x722};
    constexpr static uint16_t configure_commutation_parameters_id{0x723};
    constexpr static uint16_t stop_control_id{0x724};
    constexpr static uint16_t control_parameters_id{0x725};
    constexpr static uint16_t start_space_vector_id{0x726};
    constexpr static uint16_t fix_dc_link_voltage_id{0x727};
    constexpr static uint16_t unfix_dc_link_voltage_id{0x728};
    constexpr static uint16_t dc_link_id{0x729};

   public:
    StateMachine::state_id master_general_state{0};
    StateMachine::state_id master_nested_state{0};
    StateMachine::state_id slave_general_state{0};
    StateMachine::state_id slave_nested_state{0};

    float duty_cycle_u{0.0f};
    float duty_cycle_v{0.0f};
    float duty_cycle_w{0.0f};

    float average_dc_link_voltage{0.0f};
    float dc_link_voltage_1{0.0f};
    float dc_link_voltage_2{0.0f};
    float dc_link_voltage_3{0.0f};
    float dc_link_voltage_4{0.0f};

    uint8_t can_id;

    CMS::Communications::ModuleCAN module_can;

    CAN();

    void start();

    void restart();

    void set_data_rate(CMS::Types::TxCycle_t tx_cycle);

    void update();

    void transmit_start_test_pwm(float duty_cycle_u, float duty_cycle_v,
                                 float duty_cycle_w);
    void transmit_configure_commutation_parameters(
        uint32_t commutation_frequency_hz, uint32_t dead_time_ns);
    void transmit_stop();

    void transmit_start_space_vector(float modulation_index,
                                     float modulation_frequency_hz);

    void transmit_fix_dc_link_voltage(float voltage);
    void transmit_unfix_dc_link_voltage();
};

};  // namespace HVSCU::Communication