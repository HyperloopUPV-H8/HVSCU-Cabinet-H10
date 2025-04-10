#include "HVSCU/Communication/Ethernet.hpp"

namespace HVSCU::Communication {

Ethernet::Ethernet(float *total_supercaps_voltage,
                   std::array<std::array<float *, 48>, 3> cells_voltage,
                   std::array<float *, 3> module_voltage,
                   std::array<float *, 3> max_cell_voltage,
                   std::array<float *, 3> min_cell_voltage,
                   std::array<float *, 3> avg_cell_voltage,
                   std::array<float *, 3> max_temp,
                   std::array<float *, 3> min_temp, PinState *sdc_good,
                   float *bus_voltage,
                   Actuators::Contactors::State *contactors_internal_state,
                   float &charge_voltage, uint8_t &duty_u, uint8_t &duty_v,
                   uint8_t &duty_w, float &angular_velocity, uint16_t &ref_q,
                   uint16_t &ref_d, float &ref_speed, float &ref_switch_freq,
                   float &ref_dead_time, float &fixed_dc_link_vtg)
    : control_station_tcp(local_ip, tcp_server_port),
      control_station_udp(local_ip, udp_port, control_station_ip, udp_port),
      sdc_good(sdc_good),
      bus_voltage(bus_voltage),
      contactors_internal_state(contactors_internal_state),
      total_supercaps_voltage(total_supercaps_voltage),
      cells_voltage(cells_voltage),
      module_voltage(module_voltage),
      max_cell_voltage(max_cell_voltage),
      min_cell_voltage(min_cell_voltage),
      avg_cell_voltage(avg_cell_voltage),
      max_temp(max_temp),
      min_temp(min_temp),
      charge_voltage(charge_voltage),
      duty_u(duty_u),
      duty_v(duty_v),
      duty_w(duty_w),
      angular_velocity(angular_velocity),
      ref_q(ref_q),
      ref_d(ref_d),
      ref_speed(ref_speed),
      ref_switch_freq(ref_switch_freq),
      ref_dead_time(ref_dead_time),
      fixed_dc_link_vtg(fixed_dc_link_vtg) {

    ///////////BCU orders///////////
    BCU_test_pwm = new HeapOrder{1700, on_BCU_test_pwm,&duty_u,&duty_v,&duty_w};
    BCU_emulate_movement = new HeapOrder{1701, on_BCU_emulate_movement, &angular_velocity, &ref_q, &ref_d};
    BCU_current_control = new HeapOrder{1702, on_BCU_current_control, &ref_q, &ref_d};
    BCU_velocity_control = new HeapOrder{1703, on_BCU_velocity_control, &ref_speed};
    BCU_set_pwm_params = new HeapOrder{1704, on_BCU_set_pwm_params, &ref_switch_freq, &ref_dead_time};
    BCU_stop = new HeapOrder{1705, on_BCU_stop};
    BCU_set_fixed_dc_link_vtg = new HeapOrder{1706, on_BCU_set_fixed_dc_link_vtg, &fixed_dc_link_vtg};
    BCU_unset_fixed_dc_link_vtg = new HeapOrder{1707, on_BCU_unset_fixed_dc_link_vtg};


    ///////////BCU data///////////
    state_machine_data = new HeapPacket{1708, &BCU_data::master_general_state, &BCU_data::master_nested_state, &BCU_data::slave_general_state, &BCU_data::slave_nested_state};
    encoders_data = new HeapPacket{1709, BCU_data::encoders_data.data()};
    encoders_control = new HeapPacket{1710, BCU_data::encoders_control.data()};
    inverters_data = new HeapPacket{1711, BCU_data::inverters_data.data()};
    inverters_control = new HeapPacket{1712, &BCU_data::direction_speetec_1, &BCU_data::direction_speetec_2, &BCU_data::direction_speetec_3,&BCU_data::pod_in_booster_section};
    control_params = new HeapPacket{1713, BCU_data::control_params.data()};

      }

void Ethernet::send_supercaps_data() {
    control_station_udp.send_packet(total_voltage);
    control_station_udp.send_packet(module_1_summary);
    control_station_udp.send_packet(module_2_summary);
    control_station_udp.send_packet(module_3_summary);
    control_station_udp.send_packet(module_1_cells);
    control_station_udp.send_packet(module_2_cells);
    control_station_udp.send_packet(module_3_cells);
}

void Ethernet::send_sdc_data() { control_station_udp.send_packet(sdc_state); }

void Ethernet::send_contactors_data() {
    control_station_udp.send_packet(contactors_state);
}
void Ethernet::send_BCU_data() {
    control_station_udp.send_packet(*state_machine_data);
    control_station_udp.send_packet(*encoders_data);
    control_station_udp.send_packet(*encoders_control);
    control_station_udp.send_packet(*inverters_data);
    control_station_udp.send_packet(*inverters_control);
    control_station_udp.send_packet(*control_params);
}
bool Ethernet::is_connected() { return control_station_tcp.is_connected(); }
};  // namespace HVSCU::Communication