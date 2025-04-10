#pragma once

#include "CMS-LIB.hpp"
#include "ST-LIB.hpp"

namespace HVSCU::Communication {

class CAN {
    uint8_t can_id;

    inline constexpr static uint8_t DLC_to_length(
        const ::FDCAN::DLC dlc) noexcept;

    inline constexpr static ::FDCAN::DLC length_to_DLC(
        const uint8_t length) noexcept;

   public:
    CMS::Communications::ModuleCAN module_can;

    CAN();

    void start();

    void restart();

    void set_data_rate(CMS::Types::TxCycle_t tx_cycle);

    void update();
    
    FDCAN::Packet test_pwm{.identifier = 0x801,.data_length = FDCAN::DLC::BYTES_3};
    FDCAN::Packet emulate_movement{.identifier = 0x802,.data_length = FDCAN::DLC::BYTES_6};
    FDCAN::Packet current_control{
        .identifier = 0x803,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet velocity_control{
        .identifier = 0x804,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet set_pwm_params{
        .identifier = 0x805,
        .data_length = FDCAN::DLC::BYTES_8};
    FDCAN::Packet set_fixed_dc_link_vtg{
        .identifier = 0x806,
        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet unset_fixed_dc_link_vtg{
        .identifier = 0x807,
        .data_length = FDCAN::DLC::BYTES_0};
};

};  // namespace HVSCU::Communication