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
      fixed_dc_link_vtg(fixed_dc_link_vtg) {}

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

bool Ethernet::is_connected() { return control_station_tcp.is_connected(); }
};  // namespace HVSCU::Communication