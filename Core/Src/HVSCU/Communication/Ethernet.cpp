#include "HVSCU/Communication/Ethernet.hpp"

namespace HVSCU::Communication {

Ethernet::Ethernet(
    float *total_supercaps_voltage,
    std::array<std::array<float *, 48>, 3> cells_voltage,
    std::array<float *, 3> module_voltage,
    std::array<float *, 3> max_cell_voltage,
    std::array<float *, 3> min_cell_voltage,
    std::array<float *, 3> avg_cell_voltage, std::array<float *, 3> max_temp,
    std::array<float *, 3> min_temp, PinState *sdc_good, float *bus_voltage,
    Actuators::Contactors::State *contactors_internal_state,
    float *output_current, StateMachine::state_id *master_general_state,
    StateMachine::state_id *master_nested_state,
    StateMachine::state_id *slave_general_state,
    StateMachine::state_id *slave_nested_state, float *duty_cycle_u,
    float *duty_cycle_v, float *duty_cycle_w, float *average_dc_link_voltage,
    float *dc_link_voltage_1, float *dc_link_voltage_2,
    float *dc_link_voltage_3, float *dc_link_voltage_4,
    double *average_current_u, double *average_current_v,
    double *average_current_w)
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
      output_current(output_current),
      master_general_state(master_general_state),
      master_nested_state(master_nested_state),
      slave_general_state(slave_general_state),
      slave_nested_state(slave_nested_state),
      duty_cycle_u(duty_cycle_u),
      duty_cycle_v(duty_cycle_v),
      duty_cycle_w(duty_cycle_w),
      average_dc_link_voltage(average_dc_link_voltage),
      dc_link_voltage_1(dc_link_voltage_1),
      dc_link_voltage_2(dc_link_voltage_2),
      dc_link_voltage_3(dc_link_voltage_3),
      dc_link_voltage_4(dc_link_voltage_4),
      average_current_u(average_current_u),
      average_current_v(average_current_v),
      average_current_w(average_current_w) {}

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

void Ethernet::send_current_sense() {
    control_station_udp.send_packet(current_sense);
}

void Ethernet::send_bcu_data() {
    control_station_udp.send_packet(bcu_state_packet);
    control_station_udp.send_packet(bcu_control_parameters_packet);
    control_station_udp.send_packet(bcu_dc_link_voltage_packet);
    control_station_udp.send_packet(bcu_current_sense_packet);
}

bool Ethernet::is_connected() { return control_station_tcp.is_connected(); }
};  // namespace HVSCU::Communication