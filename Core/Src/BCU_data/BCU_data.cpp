#include "BCU_data/BCU_data.hpp"
float BCU_data::charge_voltage;
uint8_t BCU_data::duty_u;
uint8_t BCU_data::duty_v;
uint8_t BCU_data::duty_w;
float BCU_data::angular_velocity;
uint16_t BCU_data::ref_q;
uint16_t BCU_data::ref_d;
float BCU_data::ref_speed;
float BCU_data::ref_switch_freq;
float BCU_data::ref_dead_time;
float BCU_data::fixed_dc_link_vtg;

// DATA
uint8_t BCU_data::master_general_state;
uint8_t BCU_data::master_nested_state;
uint8_t BCU_data::slave_general_state;
uint8_t BCU_data::slave_nested_state;
std::array<float, 25> BCU_data::control_params;
std::array<float, 16> BCU_data::inverters_data;
std::array<float, 12> BCU_data::encoders_data;
std::array<uint8_t, 8> BCU_data::encoders_control;
uint8_t BCU_data::direction_speetec_1;
uint8_t BCU_data::direction_speetec_2;
uint8_t BCU_data::direction_speetec_3;
bool BCU_data::pod_in_booster_section;