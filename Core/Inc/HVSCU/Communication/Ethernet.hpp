#pragma once

#include "ST-LIB.hpp"

namespace HVSCU::Communication {

class Ethernet {
    ServerSocket control_station_tcp;
    DatagramSocket control_station_udp;

    HeapOrder close_contactors{999, on_close_contactors};
    HeapOrder open_contactors{998, on_open_contactors};
    HeapOrder charge_supercaps{997, on_charge_supercaps, &charge_voltage};

    static void on_close_contactors() { has_received_close_contactors = true; }
    static void on_open_contactors() { has_received_open_contactors = true; }
    static void on_charge_supercaps() { has_received_charge_supercaps = true; }

   public:
    inline static const IPV4 local_ip{"192.168.2.10"};
    inline static const IPV4 control_station_ip{"192.168.0.9"};

    inline static const uint16_t tcp_server_port{50500};
    inline static const uint16_t tcp_client_port{50401};
    inline static const uint16_t udp_port{50400};

    inline static bool has_received_close_contactors{false};
    inline static bool has_received_open_contactors{false};
    inline static bool has_received_charge_supercaps{false};

    float charge_voltage{0.0f};

    Ethernet();
};

};  // namespace HVSCU::Communication