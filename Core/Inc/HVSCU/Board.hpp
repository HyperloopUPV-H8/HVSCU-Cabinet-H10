#pragma once

#include "CMS-LIB.hpp"
#include "HVSCU/Actuators/Contactors.hpp"
#include "HVSCU/Actuators/LEDs.hpp"
#include "HVSCU/Actuators/SDC.hpp"
#include "HVSCU/Communication/CAN.hpp"
#include "HVSCU/Communication/Ethernet.hpp"
#include "HVSCU/Pinout.hpp"
#include "HVSCU/Sensors/BusVoltage.hpp"
#include "HVSCU/Sensors/CurrentSense.hpp"
#include "HVSCU/Sensors/IMD.hpp"
#include "ST-LIB.hpp"

namespace HVSCU {

struct ProtectionManagerHandle {
    ProtectionManagerHandle(StateMachine& general_state_machine,
                            ProtectionManager::state_id fault_id) {
        ProtectionManager::initialize();
        ProtectionManager::link_state_machine(general_state_machine, fault_id);
        ProtectionManager::add_standard_protections();
    }

    void update_low_frequency() { ProtectionManager::check_protections(); }

    void update_high_frequency() {
        ProtectionManager::check_high_frequency_protections();
    }
};

// Turn the STLIB into a class
struct STLIBHandle {
    STLIBHandle(string ip = "192.168.1.4", string subnet_mask = "255.255.0.0",
                string gateway = "192.168.1.1",
                UART::Peripheral& printf_peripheral = UART::uart2) {
        STLIB::start(ip, subnet_mask, gateway, printf_peripheral);
    }

    void update() { STLIB::update(); }
};

class Board {
    enum States : uint8_t { CONNECTING = 0, OPERATIONAL, FAULT };

    StateMachine general_state_machine{States::CONNECTING};

    ProtectionManagerHandle protection_manager{general_state_machine,
                                               States::FAULT};

    Sensors::IMD imd;
    Sensors::BusVoltage supercaps_voltage;
    Sensors::CurrentSense current_sense;

    float ess_voltage{0.0f};
    float bus_voltage_value{0.0f};

    Actuators::Contactors contactors;
    Actuators::LEDs leds;
    Actuators::SDC sdc;

    Communication::CAN can;

    STLIBHandle stlib;

    Communication::Ethernet ethernet;

    void populate_state_machine();
    void initialize_protections();

    void update_connecting();
    void update_operational();
    void update_fault();

    volatile bool send_ethernet_1khz{false};
    volatile bool send_ethernet_10hz{false};
    volatile bool read_sensors_10hz{false};
    volatile bool read_sensors_1khz{false};

   public:
    Board();

    void update();
};

};  // namespace HVSCU