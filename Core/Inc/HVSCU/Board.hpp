#pragma once

#include "CMS-LIB.hpp"
#include "HVSCU/Actuators/Contactors.hpp"
#include "HVSCU/Actuators/LEDs.hpp"
#include "HVSCU/Actuators/SDC.hpp"
#include "HVSCU/Communication/CAN.hpp"
#include "HVSCU/Communication/Ethernet.hpp"
#include "HVSCU/Pinout.hpp"
#include "HVSCU/Sensors/BusVoltage.hpp"
#include "HVSCU/Sensors/IMD.hpp"
#include "ST-LIB.hpp"

namespace HVSCU {

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
    enum States : uint8_t { Connecting = 0, Operational = 1, Fault = 2 };
    StateMachine general_state_machine;

    Sensors::IMD imd;
    Sensors::BusVoltage bus_voltage;

    Actuators::Contactors contactors;
    Actuators::LEDs leds;
    Actuators::SDC sdc;

    Communication::CAN can;

    STLIBHandle stlib;

    Communication::Ethernet ethernet;

    void initialize_state_machine();

    void update_connecting();
    void update_operational();
    void update_fault();

   public:
    Board();

    void update();
};

};  // namespace HVSCU