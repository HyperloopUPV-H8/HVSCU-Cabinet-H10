#pragma once
#include <cstdint>
namespace BCU_data{
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

}