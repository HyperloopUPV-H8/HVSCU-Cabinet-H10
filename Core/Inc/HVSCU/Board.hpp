#pragma once

#include "CMS-LIB.hpp"
#include "HVSCU/Actuators/Contactors.hpp"
#include "HVSCU/Actuators/LEDs.hpp"
#include "HVSCU/Communication/CAN.hpp"
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
    Actuators::LEDs leds;

    void initialize_state_machine();

   public:
    Board();

    void update();
};

};  // namespace HVSCU