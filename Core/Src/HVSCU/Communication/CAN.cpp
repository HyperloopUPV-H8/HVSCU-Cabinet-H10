#include "HVSCU/Communication/CAN.hpp"

namespace HVSCU::Communication {

CAN::CAN()
    : can_id(FDCAN::inscribe(FDCAN::fdcan1)),
      module_can(
          [&](CMS::Messages::CanPacket& packet) {
              static FDCAN::Packet last_packet{};

              bool got_something = FDCAN::read(can_id, &last_packet);

              if (got_something) {
                  packet.id = last_packet.identifier;
                  packet.length = DLC_to_length(last_packet.data_length);
                  packet.payload = last_packet.rx_data;
              }

              return got_something;
          },
          [&](CMS::Messages::CanPacket&& packet) {
              bool sent = FDCAN::transmit(
                  can_id, packet.id,
                  reinterpret_cast<const char*>(packet.payload.data()),
                  length_to_DLC(packet.length));
              return sent;
          }) {}

void CAN::start() {
    FDCAN::start();
    module_can.start_module(1, 1);
    module_can.start_module(1, 2);
    module_can.start_module(1, 3);
}

void CAN::restart() {
    module_can.restart_module(1, 1);
    module_can.restart_module(1, 2);
    module_can.restart_module(1, 3);

    Time::set_timeout(1000, [&]() { start(); });
}

void CAN::set_data_rate(CMS::Types::TxCycle_t tx_cycle) {
    module_can.change_module_tx_cycle(1, 1, tx_cycle);
    module_can.change_module_tx_cycle(1, 2, tx_cycle);
    module_can.change_module_tx_cycle(1, 3, tx_cycle);
}

void CAN::update() { module_can.update(); }

inline constexpr uint8_t CAN::DLC_to_length(const ::FDCAN::DLC dlc) noexcept {
    switch (dlc) {
        case ::FDCAN::DLC::BYTES_0:
            return 0;
        case ::FDCAN::DLC::BYTES_1:
            return 1;
        case ::FDCAN::DLC::BYTES_2:
            return 2;
        case ::FDCAN::DLC::BYTES_3:
            return 3;
        case ::FDCAN::DLC::BYTES_4:
            return 4;
        case ::FDCAN::DLC::BYTES_5:
            return 5;
        case ::FDCAN::DLC::BYTES_6:
            return 6;
        case ::FDCAN::DLC::BYTES_7:
            return 7;
        case ::FDCAN::DLC::BYTES_8:
            return 8;
        case ::FDCAN::DLC::BYTES_12:
            return 12;
        case ::FDCAN::DLC::BYTES_16:
            return 16;
        case ::FDCAN::DLC::BYTES_20:
            return 20;
        case ::FDCAN::DLC::BYTES_24:
            return 24;
        case ::FDCAN::DLC::BYTES_32:
            return 32;
        case ::FDCAN::DLC::BYTES_48:
            return 48;
        case ::FDCAN::DLC::BYTES_64:
            return 64;
        default:
            return 0;
    }
}

inline constexpr ::FDCAN::DLC CAN::length_to_DLC(
    const uint8_t length) noexcept {
    switch (length) {
        case 0:
            return ::FDCAN::DLC::BYTES_0;
        case 1:
            return ::FDCAN::DLC::BYTES_1;
        case 2:
            return ::FDCAN::DLC::BYTES_2;
        case 3:
            return ::FDCAN::DLC::BYTES_3;
        case 4:
            return ::FDCAN::DLC::BYTES_4;
        case 5:
            return ::FDCAN::DLC::BYTES_5;
        case 6:
            return ::FDCAN::DLC::BYTES_6;
        case 7:
            return ::FDCAN::DLC::BYTES_7;
        case 8:
            return ::FDCAN::DLC::BYTES_8;
        case 12:
            return ::FDCAN::DLC::BYTES_12;
        case 16:
            return ::FDCAN::DLC::BYTES_16;
        case 20:
            return ::FDCAN::DLC::BYTES_20;
        case 24:
            return ::FDCAN::DLC::BYTES_24;
        case 32:
            return ::FDCAN::DLC::BYTES_32;
        case 48:
            return ::FDCAN::DLC::BYTES_48;
        case 64:
            return ::FDCAN::DLC::BYTES_64;
        default:
            return ::FDCAN::DLC::DEFAULT;
    }
}

};  // namespace HVSCU::Communication