#pragma once

#include "CMS-LIB.hpp"
#include "ST-LIB.hpp"
#include "BCU_data/BCU_data.hpp"
namespace HVSCU::Communication {

class CAN {

    inline constexpr static uint8_t DLC_to_length(
        const ::FDCAN::DLC dlc) noexcept;

    inline constexpr static ::FDCAN::DLC length_to_DLC(
        const uint8_t length) noexcept;

   public:
   uint8_t can_id;

    CMS::Communications::ModuleCAN module_can;

    CAN();

    void start();

    void restart();

    void set_data_rate(CMS::Types::TxCycle_t tx_cycle);

    void update();
    void handle_bcu_data(FDCAN::Packet& packet); 
    FDCAN::Packet test_pwm{.identifier = 0x701,.data_length = FDCAN::DLC::BYTES_3};
    FDCAN::Packet emulate_movement{.identifier = 0x702,.data_length = FDCAN::DLC::BYTES_6};
    FDCAN::Packet current_control{
        .identifier = 0x703,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet velocity_control{
        .identifier = 0x704,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet set_pwm_params{
        .identifier = 0x705,
        .data_length = FDCAN::DLC::BYTES_8};
    FDCAN::Packet set_fixed_dc_link_vtg{
        .identifier = 0x706,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet unset_fixed_dc_link_vtg{
        .identifier = 0x707,
        .data_length = FDCAN::DLC::BYTES_0};
    FDCAN::Packet stop{
        .identifier = 0x708,
        .data_length = FDCAN::DLC::BYTES_0};
};

};  // namespace HVSCU::Communication