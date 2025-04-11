#pragma once

#include "BCU_data/BCU_data.hpp"
#include "CMS-LIB.hpp"
#include "ST-LIB.hpp"
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
    FDCAN::Packet test_pwm{.identifier = 0x721,
                           .data_length = FDCAN::DLC::BYTES_3};
    FDCAN::Packet emulate_movement{.identifier = 0x722,
                                   .data_length = FDCAN::DLC::BYTES_6};
    FDCAN::Packet current_control{.identifier = 0x723,
                                  .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet velocity_control{.identifier = 0x724,
                                   .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet set_pwm_params{.identifier = 0x725,
                                 .data_length = FDCAN::DLC::BYTES_8};
    FDCAN::Packet set_fixed_dc_link_vtg{.identifier = 0x726,
                                        .data_length = FDCAN::DLC::BYTES_4};
    FDCAN::Packet unset_fixed_dc_link_vtg{.identifier = 0x727,
                                          .data_length = FDCAN::DLC::BYTES_0};
    FDCAN::Packet stop{.identifier = 0x728, .data_length = FDCAN::DLC::BYTES_0};
};

};  // namespace HVSCU::Communication