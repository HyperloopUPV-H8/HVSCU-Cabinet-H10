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

    float *total_supercaps_voltage;
    std::array<std::array<float *, 48>, 3> cells_voltage;
    std::array<float *, 3> module_voltage;
    std::array<float *, 3> max_cell_voltage;
    std::array<float *, 3> min_cell_voltage;
    std::array<float *, 3> avg_cell_voltage;
    std::array<float *, 3> max_temp;
    std::array<float *, 3> min_temp;

    HeapPacket total_voltage{1600, total_supercaps_voltage};

    HeapPacket module_1_summary{1601,
                                module_voltage[0],
                                max_cell_voltage[0],
                                min_cell_voltage[0],
                                avg_cell_voltage[0],
                                max_temp[0],
                                min_temp[0]};

    HeapPacket module_2_summary{1602,
                                module_voltage[1],
                                max_cell_voltage[1],
                                min_cell_voltage[1],
                                avg_cell_voltage[1],
                                max_temp[1],
                                min_temp[1]};

    HeapPacket module_3_summary{1603,
                                module_voltage[2],
                                max_cell_voltage[2],
                                min_cell_voltage[2],
                                avg_cell_voltage[2],
                                max_temp[2],
                                min_temp[2]};

    HeapPacket module_1_cells{1604,
                              cells_voltage[0][0],
                              cells_voltage[0][1],
                              cells_voltage[0][2],
                              cells_voltage[0][3],
                              cells_voltage[0][4],
                              cells_voltage[0][5],
                              cells_voltage[0][6],
                              cells_voltage[0][7],
                              cells_voltage[0][8],
                              cells_voltage[0][9],
                              cells_voltage[0][10],
                              cells_voltage[0][11],
                              cells_voltage[0][12],
                              cells_voltage[0][13],
                              cells_voltage[0][14],
                              cells_voltage[0][15],
                              cells_voltage[0][16],
                              cells_voltage[0][17],
                              cells_voltage[0][18],
                              cells_voltage[0][19],
                              cells_voltage[0][20],
                              cells_voltage[0][21],
                              cells_voltage[0][22],
                              cells_voltage[0][23],
                              cells_voltage[0][24],
                              cells_voltage[0][25],
                              cells_voltage[0][26],
                              cells_voltage[0][27],
                              cells_voltage[0][28],
                              cells_voltage[0][29],
                              cells_voltage[0][30],
                              cells_voltage[0][31],
                              cells_voltage[0][32],
                              cells_voltage[0][33],
                              cells_voltage[0][34],
                              cells_voltage[0][35],
                              cells_voltage[0][36],
                              cells_voltage[0][37],
                              cells_voltage[0][38],
                              cells_voltage[0][39],
                              cells_voltage[0][40],
                              cells_voltage[0][41],
                              cells_voltage[0][42],
                              cells_voltage[0][43],
                              cells_voltage[0][44],
                              cells_voltage[0][45],
                              cells_voltage[0][46],
                              cells_voltage[0][47]};

    HeapPacket module_2_cells{1605,
                              cells_voltage[1][0],
                              cells_voltage[1][1],
                              cells_voltage[1][2],
                              cells_voltage[1][3],
                              cells_voltage[1][4],
                              cells_voltage[1][5],
                              cells_voltage[1][6],
                              cells_voltage[1][7],
                              cells_voltage[1][8],
                              cells_voltage[1][9],
                              cells_voltage[1][10],
                              cells_voltage[1][11],
                              cells_voltage[1][12],
                              cells_voltage[1][13],
                              cells_voltage[1][14],
                              cells_voltage[1][15],
                              cells_voltage[1][16],
                              cells_voltage[1][17],
                              cells_voltage[1][18],
                              cells_voltage[1][19],
                              cells_voltage[1][20],
                              cells_voltage[1][21],
                              cells_voltage[1][22],
                              cells_voltage[1][23],
                              cells_voltage[1][24],
                              cells_voltage[1][25],
                              cells_voltage[1][26],
                              cells_voltage[1][27],
                              cells_voltage[1][28],
                              cells_voltage[1][29],
                              cells_voltage[1][30],
                              cells_voltage[1][31],
                              cells_voltage[1][32],
                              cells_voltage[1][33],
                              cells_voltage[1][34],
                              cells_voltage[1][35],
                              cells_voltage[1][36],
                              cells_voltage[1][37],
                              cells_voltage[1][38],
                              cells_voltage[1][39],
                              cells_voltage[1][40],
                              cells_voltage[1][41],
                              cells_voltage[1][42],
                              cells_voltage[1][43],
                              cells_voltage[1][44],
                              cells_voltage[1][45],
                              cells_voltage[1][46],
                              cells_voltage[1][47]};

    HeapPacket module_3_cells{1606,
                              cells_voltage[2][0],
                              cells_voltage[2][1],
                              cells_voltage[2][2],
                              cells_voltage[2][3],
                              cells_voltage[2][4],
                              cells_voltage[2][5],
                              cells_voltage[2][6],
                              cells_voltage[2][7],
                              cells_voltage[2][8],
                              cells_voltage[2][9],
                              cells_voltage[2][10],
                              cells_voltage[2][11],
                              cells_voltage[2][12],
                              cells_voltage[2][13],
                              cells_voltage[2][14],
                              cells_voltage[2][15],
                              cells_voltage[2][16],
                              cells_voltage[2][17],
                              cells_voltage[2][18],
                              cells_voltage[2][19],
                              cells_voltage[2][20],
                              cells_voltage[2][21],
                              cells_voltage[2][22],
                              cells_voltage[2][23],
                              cells_voltage[2][24],
                              cells_voltage[2][25],
                              cells_voltage[2][26],
                              cells_voltage[2][27],
                              cells_voltage[2][28],
                              cells_voltage[2][29],
                              cells_voltage[2][30],
                              cells_voltage[2][31],
                              cells_voltage[2][32],
                              cells_voltage[2][33],
                              cells_voltage[2][34],
                              cells_voltage[2][35],
                              cells_voltage[2][36],
                              cells_voltage[2][37],
                              cells_voltage[2][38],
                              cells_voltage[2][39],
                              cells_voltage[2][40],
                              cells_voltage[2][41],
                              cells_voltage[2][42],
                              cells_voltage[2][43],
                              cells_voltage[2][44],
                              cells_voltage[2][45],
                              cells_voltage[2][46],
                              cells_voltage[2][47]};

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

    Ethernet(float *total_supercaps_voltage,
             std::array<std::array<float *, 48>, 3> cells_voltage,
             std::array<float *, 3> module_voltage,
             std::array<float *, 3> max_cell_voltage,
             std::array<float *, 3> min_cell_voltage,
             std::array<float *, 3> avg_cell_voltage,
             std::array<float *, 3> max_temp, std::array<float *, 3> min_temp);
};

};  // namespace HVSCU::Communication