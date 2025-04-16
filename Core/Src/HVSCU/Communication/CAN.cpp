#include "HVSCU/Communication/CAN.hpp"

namespace HVSCU::Communication {

CAN::CAN()
    : can_id(FDCAN::inscribe<
             CANBitRatesSpeed::CAN_500_kbit, CANFormat::CAN_NORMAL_FORMAT,
             CANIdentifier::CAN_11_BIT_IDENTIFIER, CANMode::CAN_MODE_NORMAL>(
          FDCAN::fdcan1)),
      module_can(
          [&](CMS::Messages::CanPacket& packet) {
              static FDCAN::Packet last_packet{};
              static uint16_t encoded_duty_cycle_u{0};
              static uint16_t encoded_duty_cycle_v{0};
              static uint16_t encoded_duty_cycle_w{0};
              static uint8_t encoded_dc_link_voltage_1{0};
              static uint8_t encoded_dc_link_voltage_2{0};
              static uint8_t encoded_dc_link_voltage_3{0};
              static uint8_t encoded_dc_link_voltage_4{0};
              static uint16_t encoded_average_current_u{0};
              static uint16_t encoded_average_current_v{0};
              static uint16_t encoded_average_current_w{0};

              bool got_something = FDCAN::read(can_id, &last_packet);

              if (got_something) {
                  switch (last_packet.identifier) {
                      case state_id:
                          memcpy(&master_general_state, &last_packet.rx_data[0],
                                 sizeof(StateMachine::state_id));
                          memcpy(&master_nested_state, &last_packet.rx_data[1],
                                 sizeof(StateMachine::state_id));
                          memcpy(&slave_general_state, &last_packet.rx_data[2],
                                 sizeof(StateMachine::state_id));
                          memcpy(&slave_nested_state, &last_packet.rx_data[3],
                                 sizeof(StateMachine::state_id));
                          break;
                      case control_parameters_id:
                          memcpy(&encoded_duty_cycle_u, &last_packet.rx_data[0],
                                 sizeof(uint16_t));
                          memcpy(&encoded_duty_cycle_v, &last_packet.rx_data[2],
                                 sizeof(uint16_t));
                          memcpy(&encoded_duty_cycle_w, &last_packet.rx_data[4],
                                 sizeof(uint16_t));
                          duty_cycle_u = (float)encoded_duty_cycle_u / 100.0f;
                          duty_cycle_v = (float)encoded_duty_cycle_v / 100.0f;
                          duty_cycle_w = (float)encoded_duty_cycle_w / 100.0f;
                          break;
                      case dc_link_id:
                          memcpy(&average_dc_link_voltage,
                                 &last_packet.rx_data[0], sizeof(float));
                          memcpy(&encoded_dc_link_voltage_1,
                                 &last_packet.rx_data[4], sizeof(uint8_t));
                          memcpy(&encoded_dc_link_voltage_2,
                                 &last_packet.rx_data[5], sizeof(uint8_t));
                          memcpy(&encoded_dc_link_voltage_3,
                                 &last_packet.rx_data[6], sizeof(uint8_t));
                          memcpy(&encoded_dc_link_voltage_4,
                                 &last_packet.rx_data[7], sizeof(uint8_t));
                          dc_link_voltage_1 = (float)encoded_dc_link_voltage_1;
                          dc_link_voltage_2 = (float)encoded_dc_link_voltage_2;
                          dc_link_voltage_3 = (float)encoded_dc_link_voltage_3;
                          dc_link_voltage_4 = (float)encoded_dc_link_voltage_4;
                          break;
                      case current_sense_id:
                          memcpy(&encoded_average_current_u,
                                 &last_packet.rx_data[0], sizeof(uint16_t));
                          memcpy(&encoded_average_current_v,
                                 &last_packet.rx_data[2], sizeof(uint16_t));
                          memcpy(&encoded_average_current_w,
                                 &last_packet.rx_data[4], sizeof(uint16_t));
                          average_current_u =
                              (double)encoded_average_current_u / 100.0;
                          average_current_v =
                              (double)encoded_average_current_v / 100.0;
                          average_current_w =
                              (double)encoded_average_current_w / 100.0;
                          break;
                      default:
                          break;
                  }

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

void CAN::transmit_start_test_pwm(float duty_cycle_u, float duty_cycle_v,
                                  float duty_cycle_w) {
    uint16_t encoded_duty_cycle_u = (uint16_t)(duty_cycle_u * 100.0f);
    uint16_t encoded_duty_cycle_v = (uint16_t)(duty_cycle_v * 100.0f);
    uint16_t encoded_duty_cycle_w = (uint16_t)(duty_cycle_w * 100.0f);

    std::array<uint8_t, 6> payload{};
    memcpy(payload.data(), &encoded_duty_cycle_u, sizeof(uint16_t));
    memcpy(payload.data() + 2, &encoded_duty_cycle_v, sizeof(uint16_t));
    memcpy(payload.data() + 4, &encoded_duty_cycle_w, sizeof(uint16_t));
    FDCAN::transmit(can_id, start_test_pwm_id,
                    reinterpret_cast<const char*>(payload.data()),
                    length_to_DLC(payload.size()));
}

void CAN::transmit_configure_commutation_parameters(
    uint32_t commutation_frequency_hz, uint32_t dead_time_ns) {
    std::array<uint8_t, 8> payload{};
    memcpy(payload.data(), &commutation_frequency_hz, sizeof(uint32_t));
    memcpy(payload.data() + 4, &dead_time_ns, sizeof(uint32_t));
    FDCAN::transmit(can_id, configure_commutation_parameters_id,
                    reinterpret_cast<const char*>(payload.data()),
                    length_to_DLC(payload.size()));
}

void CAN::transmit_stop() {
    std::array<uint8_t, 0> payload{};
    FDCAN::transmit(can_id, stop_control_id,
                    reinterpret_cast<const char*>(payload.data()),
                    FDCAN::DLC::BYTES_0);
}

void CAN::transmit_start_space_vector(float modulation_index,
                                      float modulation_frequency_hz) {
    std::array<uint8_t, 8> payload{};
    memcpy(payload.data(), &modulation_index, sizeof(float));
    memcpy(payload.data() + 4, &modulation_frequency_hz, sizeof(float));
    FDCAN::transmit(can_id, start_space_vector_id,
                    reinterpret_cast<const char*>(payload.data()),
                    length_to_DLC(payload.size()));
}

void CAN::transmit_fix_dc_link_voltage(float voltage) {
    std::array<uint8_t, 4> payload{};
    memcpy(payload.data(), &voltage, sizeof(float));
    FDCAN::transmit(can_id, fix_dc_link_voltage_id,
                    reinterpret_cast<const char*>(payload.data()),
                    length_to_DLC(payload.size()));
}

void CAN::transmit_unfix_dc_link_voltage() {
    std::array<uint8_t, 0> payload{};
    FDCAN::transmit(can_id, unfix_dc_link_voltage_id,
                    reinterpret_cast<const char*>(payload.data()),
                    FDCAN::DLC::BYTES_0);
}

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