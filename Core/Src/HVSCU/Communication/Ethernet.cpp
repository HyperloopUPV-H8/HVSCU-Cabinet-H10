#include "HVSCU/Communication/Ethernet.hpp"

namespace HVSCU::Communication {

Ethernet::Ethernet()
    : control_station_tcp(local_ip, tcp_server_port),
      control_station_udp(local_ip, udp_port, control_station_ip, udp_port) {}

};  // namespace HVSCU::Communication