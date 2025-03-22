#include "HVSCU/Communication/Ethernet.hpp"

namespace HVSCU::Communication {

Ethernet::Ethernet(float *total_supercaps_voltage,
                   std::array<std::array<float *, 48>, 3> cells_voltage,
                   std::array<float *, 3> module_voltage,
                   std::array<float *, 3> max_cell_voltage,
                   std::array<float *, 3> min_cell_voltage,
                   std::array<float *, 3> avg_cell_voltage,
                   std::array<float *, 3> max_temp,
                   std::array<float *, 3> min_temp)
    : control_station_tcp(local_ip, tcp_server_port),
      control_station_udp(local_ip, udp_port, control_station_ip, udp_port),
      total_supercaps_voltage(total_supercaps_voltage),
      cells_voltage(cells_voltage),
      module_voltage(module_voltage),
      max_cell_voltage(max_cell_voltage),
      min_cell_voltage(min_cell_voltage),
      avg_cell_voltage(avg_cell_voltage),
      max_temp(max_temp),
      min_temp(min_temp) {}

};  // namespace HVSCU::Communication