#pragma once
#include <cstdint>
#include <array>
namespace BCU_data{
    //ORDERS
    extern float charge_voltage;
    extern uint8_t duty_u;
    extern uint8_t duty_v;
    extern uint8_t duty_w;
    extern float angular_velocity;
    extern uint16_t ref_q;
    extern uint16_t ref_d;
    extern float ref_speed;
    extern float ref_switch_freq;
    extern float ref_dead_time;
    extern float fixed_dc_link_vtg;

    // DATA
    extern uint8_t master_general_state;
    extern uint8_t master_nested_state;
    extern uint8_t slave_general_state;
    extern uint8_t slave_nested_state;
    extern std::array<float,25> control_params;
    extern std::array<float,16> inverters_data;
    extern std::array<float,12> encoders_data;
    extern std::array<uint8_t,8> encoders_control;
    extern uint8_t direction_speetec_1;
    extern uint8_t direction_speetec_2;
    extern uint8_t direction_speetec_3;
    extern bool pod_in_booster_section;

}