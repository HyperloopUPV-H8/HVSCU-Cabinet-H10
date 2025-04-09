#ifndef SIM_ON
#include "main.h"

#include "lwip.h"
#endif

#include "HVSCU/Board.hpp"

int main(void) {
#ifdef SIM_ON
    SharedMemory::start();
#endif

    HVSCU::Board hvscu;

    while (1) {
        hvscu.update();
    }
}

void Error_Handler(void) {
    ErrorHandler("HAL error handler triggered");
    while (1) {
    }
}
