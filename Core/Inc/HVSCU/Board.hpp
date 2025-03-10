#pragma once

#include "CMS-LIB.hpp"
#include "HVSCU/Actuators/Contactors.hpp"
#include "HVSCU/Actuators/LEDs.hpp"
#include "HVSCU/Communication/CAN.hpp"
#include "HVSCU/Communication/Ethernet.hpp"
#include "HVSCU/Pinout.hpp"
#include "HVSCU/Sensors/IMD.hpp"
#include "ST-LIB.hpp"

namespace HVSCU {

class Board {
    float bus_voltage{0.0};

    enum States : uint8_t { Connecting = 0, Operational = 1, Fault = 2 };
    StateMachine general_state_machine;

    Communication::CAN can;

    Sensors::IMD imd;

    Actuators::Contactors contactors;

    // This is here to avoid initialization reorder warning and still be able to
    // give it the sensor data for udp in the initialization list
    Communication::Ethernet ethernet;

    Actuators::LEDs leds;

    void initialize_state_machine();

    void update_connecting();
    void update_operational();
    void update_fault();

   public:
    Board();

    void update();
};

};  // namespace HVSCU