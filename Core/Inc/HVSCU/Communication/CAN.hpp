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
};

};  // namespace HVSCU::Communication