#pragma once

#include "HVSCU/Actuators/Contactors.hpp"
#include "ST-LIB.hpp"

namespace HVSCU::Communication {

class Ethernet {
   public:
    enum ImdBypassState : uint8_t {
        EnableIMD = 0,
        DisableIMD = 1,
    };

    enum SdcEnableState : uint8_t {
        EnableSDC = 0,
        DisableSDC = 1,
    };

    inline static const IPV4 local_ip{"192.168.2.16"};
    inline static const IPV4 control_station_ip{"192.168.0.9"};

    inline static const uint16_t tcp_server_port{50500};
    inline static const uint16_t tcp_client_port{50401};
    inline static const uint16_t udp_port{50400};

    inline static bool has_received_close_contactors{false};
    inline static bool has_received_open_contactors{false};
    inline static bool has_received_charge_supercaps{false};
    inline static bool has_received_reset_supercaps{false};
    inline static bool has_received_imd_bypass{false};
    inline static bool has_received_sdc_enable{false};

    inline static bool has_received_BCU_test_pwm{false};
    inline static bool has_received_BCU_configure_commutation_parameters{false};
    inline static bool has_received_BCU_stop{false};
    inline static bool has_received_BCU_space_vector{false};
    inline static bool has_received_BCU_fix_dc_link_voltage{false};
    inline static bool has_received_BCU_unfix_dc_link_voltage{false};

    float charge_voltage{0.0f};
    ImdBypassState imd_bypass_state{ImdBypassState::DisableIMD};
    SdcEnableState sdc_enable_state{SdcEnableState::DisableSDC};

    float requested_duty_cycle_u{0.0f};
    float requested_duty_cycle_v{0.0f};
    float requested_duty_cycle_w{0.0f};

    uint32_t requested_commutation_frequency_hz{0};
    uint32_t requested_dead_time_ns{0};

    float requested_modulation_index{0.0f};
    float requested_modulation_frequency_hz{0.0f};

    float requested_dc_link_voltage{0.0f};

   private:
    ServerSocket control_station_tcp;
    DatagramSocket control_station_udp;

    PinState *sdc_good;
    float *bus_voltage;
    Actuators::Contactors::State *contactors_internal_state;
    float *total_supercaps_voltage;
    std::array<std::array<float *, 48>, 3> cells_voltage;
    std::array<float *, 3> module_voltage;
    std::array<float *, 3> max_cell_voltage;
    std::array<float *, 3> min_cell_voltage;
    std::array<float *, 3> avg_cell_voltage;
    std::array<float *, 3> max_temp;
    std::array<float *, 3> min_temp;
    float *output_current;

    StateMachine::state_id *master_general_state;
    StateMachine::state_id *master_nested_state;
    StateMachine::state_id *slave_general_state;
    StateMachine::state_id *slave_nested_state;
    float *duty_cycle_u;
    float *duty_cycle_v;
    float *duty_cycle_w;
    float *average_dc_link_voltage;
    float *dc_link_voltage_1;
    float *dc_link_voltage_2;
    float *dc_link_voltage_3;
    float *dc_link_voltage_4;

    static void on_close_contactors() { has_received_close_contactors = true; }

    static void on_open_contactors() { has_received_open_contactors = true; }

    static void on_charge_supercaps() { has_received_charge_supercaps = true; }

    static void on_reset_supercaps() { has_received_reset_supercaps = true; }

    static void on_imd_bypass() { has_received_imd_bypass = true; }

    static void on_sdc_enable() { has_received_sdc_enable = true; }

    HeapOrder close_contactors{1699, on_close_contactors};
    HeapOrder open_contactors{1698, on_open_contactors};
    HeapOrder charge_supercaps{1697, on_charge_supercaps, &charge_voltage};
    HeapOrder reset_supercaps{1696, on_reset_supercaps};
    HeapOrder imd_bypass{1695, on_imd_bypass, &imd_bypass_state};
    HeapOrder sdc_enalbe{1694, on_sdc_enable, &sdc_enable_state};

    static void on_BCU_test_pwm() { has_received_BCU_test_pwm = true; }

    static void on_BCU_configure_commutation_parameters() {
        has_received_BCU_configure_commutation_parameters = true;
    }

    static void on_BCU_stop() { has_received_BCU_stop = true; }

    static void on_BCU_space_vector() { has_received_BCU_space_vector = true; }

    static void on_BCU_fix_dc_link_voltage() {
        has_received_BCU_fix_dc_link_voltage = true;
    }
    static void on_BCU_unfix_dc_link_voltage() {
        has_received_BCU_unfix_dc_link_voltage = true;
    }

    HeapOrder BCU_test_pwm{1799, on_BCU_test_pwm, &requested_duty_cycle_u,
                           &requested_duty_cycle_v, &requested_duty_cycle_w};

    HeapOrder BCU_configure_commutation_parameters{
        1795, on_BCU_configure_commutation_parameters,
        &requested_commutation_frequency_hz, &requested_dead_time_ns};

    HeapOrder BCU_stop{1794, on_BCU_stop};

    HeapOrder BCU_space_vector{1793, on_BCU_space_vector,
                               &requested_modulation_index,
                               &requested_modulation_frequency_hz};

    HeapOrder BCU_fix_dc_link_voltage{1795, on_BCU_fix_dc_link_voltage,
                                      &requested_dc_link_voltage};
    HeapOrder BCU_unfix_dc_link_voltage{1796, on_BCU_unfix_dc_link_voltage};

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

    HeapPacket sdc_state{1607, sdc_good};

    HeapPacket contactors_state{1608, bus_voltage, contactors_internal_state};

    HeapPacket current_sense{1609, output_current};

    HeapPacket bcu_state_packet{1701, master_general_state, master_nested_state,
                                slave_general_state, slave_nested_state};

    HeapPacket bcu_control_parameters_packet{1700, duty_cycle_u, duty_cycle_v,
                                             duty_cycle_w};

    HeapPacket bcu_dc_link_voltage_packet{1702,
                                          average_dc_link_voltage,
                                          dc_link_voltage_1,
                                          dc_link_voltage_2,
                                          dc_link_voltage_3,
                                          dc_link_voltage_4};

   public:
    Ethernet(float *total_supercaps_voltage,
             std::array<std::array<float *, 48>, 3> cells_voltage,
             std::array<float *, 3> module_voltage,
             std::array<float *, 3> max_cell_voltage,
             std::array<float *, 3> min_cell_voltage,
             std::array<float *, 3> avg_cell_voltage,
             std::array<float *, 3> max_temp, std::array<float *, 3> min_temp,
             PinState *sdc_good, float *bus_voltage,
             Actuators::Contactors::State *contactors_internal_state,
             float *output_current,
             StateMachine::state_id *master_general_state,
             StateMachine::state_id *master_nested_state,
             StateMachine::state_id *slave_general_state,
             StateMachine::state_id *slave_nested_state, float *duty_cycle_u,
             float *duty_cycle_v, float *duty_cycle_w,
             float *average_dc_link_voltage, float *dc_link_voltage_1,
             float *dc_link_voltage_2, float *dc_link_voltage_3,
             float *dc_link_voltage_4);

    void send_supercaps_data();
    void send_sdc_data();
    void send_contactors_data();
    void send_current_sense();
    void send_bcu_data();
    bool is_connected();
};

};  // namespace HVSCU::Communication